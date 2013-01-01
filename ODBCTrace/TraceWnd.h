#include "INI_File.h"
#include "..\Common\FloatState.h"
#include "OptionsDlg.h"
#include "ODBCTrace.h"
#include "StatementInfo.h"

extern HINSTANCE g_hInstance;

extern TCHAR g_szAppPath[MAX_PATH], *g_szAppName;

extern const CINI_File *pAppProfile;

STDAPI GetProfilePath(LPTSTR path, BOOL fCreate);

class CDateTimeString
{
public:
	TCHAR date[sizeof "yyyy-MM-dd"];
	TCHAR time[sizeof "HH:mm:ss"];
	CDateTimeString()
	{
		SYSTEMTIME st;
		::GetLocalTime(&st);
		::GetDateFormat(LOCALE_SYSTEM_DEFAULT, 0, &st, TEXT("yyyy-MM-dd"), date, sizeof "yyyy-MM-dd");
		::GetTimeFormat(LOCALE_SYSTEM_DEFAULT, 0, &st, TEXT("HH:mm:ss"), time, sizeof "HH:mm:ss");
	}
};

class CPerformanceCounterString
{
public:
	static LARGE_INTEGER frequency;
	static void InitializeClass()
	{
		if (!::QueryPerformanceFrequency(&frequency) || frequency.HighPart || (LONG)frequency.LowPart <= 0)
		{
			frequency.LowPart = 1000;
			frequency.HighPart = 1;
		}
	}
	static void GetCounter(LARGE_INTEGER &counter)
	{
		if (frequency.HighPart)
		{
			counter.LowPart = ::GetTickCount();
			counter.HighPart = 0;
		}
		else
		{
			::QueryPerformanceCounter(&counter);
		}
	}
	TCHAR text[20];
	static LONG QueryElapsed(const LARGE_INTEGER &lower)
	{
		LARGE_INTEGER upper;
		GetCounter(upper);
		upper.QuadPart -= lower.QuadPart;
		if (upper.HighPart || (LONG)upper.LowPart < 0)
		{
			upper.LowPart = 0xFFFFFFFF;
		}
		return upper.LowPart;
	}
	CPerformanceCounterString(LONG LowPart)
	{
		LONG elapsed = MulDiv(LowPart & 0x7FFFFFFF, 1000000, frequency.LowPart);
		wsprintf(text, &TEXT(">%lu µs")[LowPart >= 0], elapsed);
	}
};

class CTraceWnd : public CFloatFlags
{
public:
	static const TCHAR szFileTitle[];
	static DWORD dwThreadId;
	static HANDLE hEventInitComplete;
	static HANDLE hThread;
	static void InitializeClass();
	static DWORD WINAPI ThreadProc(LPVOID);
	static COLORREF rgbEnter;
	static COLORREF rgbLeave;
	static COLORREF rgbError;
	static COLORREF rgbParam;
	static COLORREF rgbSqlEx;
	static COLORREF rgbDebug;
	static COLORREF rgbTlftm;
	static COLORREF rgbBkgnd;
	static COLORREF rgbPopup;
protected:
	static HWND hwndMain;
	static HWND hwndEdit;
	static HWND hwndHead;
	static HWND hwndFind;
	static HWND hwndTip;
	static UINT commdlg_FindReplace;
	static BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
	static BOOL CALLBACK DlgProcAbout(HWND, UINT, WPARAM, LPARAM);
	static void OnInitDialog();
	static void OnSelchangeCbFont();
	static void OnFindReplace(FINDREPLACE *);
	static void ShowTip(BOOL);
	static void OnContextMenu(HWND, int, int);
	static struct fr : FINDREPLACE
	{
		enum { wBufLen = 200 };
		TCHAR szFindWhat[wBufLen];
		TCHAR szReplaceWith[wBufLen];
	} fr;
	static void WaitForClose(LPCTSTR, UINT);
	static void WINAPI ExitProcess(UINT);
	static void WINAPI TerminateProcess(HANDLE, UINT);
	static void HookProcessTermination(HMODULE);
	static void HookProcessTermination();
	static void WINAPI OutputDebugStringA(LPCSTR);
	static void WINAPI OutputDebugStringW(LPCWSTR);
	static void HookOutputDebugString(HMODULE);
	static void HookOutputDebugString();
	enum { dwDefaultEventMaskEdit = ENM_UPDATE | ENM_KEYEVENTS | ENM_MOUSEEVENTS };
};

class TraceRaw : public CTraceMsg
{
public:
	CHARFORMAT cf;
	LPCTSTR text;
	TraceRaw(COLORREF crTextColor, LPCTSTR text, SENDASYNCPROC lpCallBack = 0): text(text)
	{
		cf.cbSize = sizeof cf;
		cf.dwMask = CFM_COLOR;
		cf.crTextColor = crTextColor;
		cf.dwEffects = 0;
		Send(lpCallBack);
	}
	virtual void STDAPICALLTYPE Dump(HWND);
};

class __declspec(novtable) CallFrame
{
public:
	CallFrame *next;
	CRITICAL_SECTION *pcs;
	RETCODE depth;
	const short function_id;
	const char *const function_name;
	LARGE_INTEGER time;
	SQLSMALLINT HandleType;
	SQLSMALLINT HandleOffset;
	CallFrame(const short function_id, const char *const function_name, SQLSMALLINT HandleType = 0, SQLSMALLINT HandleOffset = 0);
	~CallFrame();
	virtual void Dump() = 0;
	virtual char *DumpDiagFields(RETCODE)
	{
		return 0;
	}
	virtual void DumpResults()
	{
	}
	void Dump(const char *fmt)
	{
		TraceFmtV(CTraceWnd::rgbParam, fmt, va_list(this + 1));
	}
	static void DumpResults(const char *fmt, ...)
	{
		TraceFmtV(CTraceWnd::rgbParam, fmt, va_list(&fmt + 1));
	}
};

class CallStack
{
public:
	static CRITICAL_SECTION cs;
	static void InitializeClass()
	{
		::InitializeCriticalSection(&cs);
	}
	CallFrame *head;
	RETCODE depth;
	BOOLEAN lockout;
	LARGE_INTEGER time;
	CallStack();
	~CallStack();
	RETCODE Push(CallFrame *);
	CallFrame *Pop(RETCODE, RETCODE);
	inline BOOLEAN HideLevel()
	{
		return lockout || ODBCTrace.fDisableTrace || depth && ODBCTrace.fHideNestedCalls;
	}
	class Ptr;
};

class CallStack::Ptr
{
public:
	DWORD dwTlsIndex;
	CallStack *operator->();
};

extern CallStack::Ptr stack;
