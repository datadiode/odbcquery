#define REFLECT_STRUCT(NAME, HEAD, BODY) \
	const char NAME[] = #BODY; struct NAME { HEAD ; BODY BYTE fUnused; }

interface IDataConvert;

REFLECT_STRUCT
(
	ODBCTraceState,
	typedef BYTE f;
	enum { cbUnused = 2 };
	// <padding>
	f rgfUnused[cbUnused];
	// </padding>
	f fTraceThreadLifetime;
	f fCatchDebugStrings;
	LONG cpHighWaterMarkFile;
	UINT nLimitRowDump;
	IDataConvert *pDataConvert;
	f fHideNestedCalls;
	f fHideDiagInfo;
	f fDisableTrace;
	f fWaitForClose;
	LONG cpHighWaterMark;
	HWND hWnd;
	UINT uMsg;
	IStream *pStream;
	f fSendToWindow;
	f fSendToFile;
	f fTopMost;
	f fSerialize;
	DWORD dwVersion,
	f SQLAllocConnect;
	f SQLAllocEnv;
	f SQLAllocStmt;
	f SQLAllocHandle;
	f SQLAllocHandleStd;
	f SQLDataSources;
	f SQLDrivers;
	f SQLBindCol;
	f SQLCancel;
	f SQLColAttributes;
	f SQLConnect;
	f SQLDescribeCol;
	f SQLDisconnect;
	f SQLError;
	f SQLExecDirect;
	f SQLExecute;
	f SQLFetch;
	f SQLFreeConnect;
	f SQLFreeEnv;
	f SQLFreeStmt;
	f SQLFreeHandle;
	f SQLGetCursorName;
	f SQLNumResultCols;
	f SQLPrepare;
	f SQLRowCount;
	f SQLSetCursorName;
	f SQLTransact;
	f SQLColumns;
	f SQLDriverConnect;
	f SQLGetEnvAttr;
	f SQLGetConnectAttr;
	f SQLGetConnectOption;
	f SQLGetData;
	f SQLGetFunctions;
	f SQLGetInfo;
	f SQLGetStmtAttr;
	f SQLGetStmtOption;
	f SQLGetTypeInfo;
	f SQLParamData;
	f SQLPutData;
	f SQLSetEnvAttr;
	f SQLSetConnectAttr;
	f SQLSetConnectOption;
	f SQLSetStmtAttr;
	f SQLSetStmtOption;
	f SQLSpecialColumns;
	f SQLStatistics;
	f SQLTables;
	f SQLBrowseConnect;
	f SQLColumnPrivileges;
	f SQLDescribeParam;
	f SQLExtendedFetch;
	f SQLForeignKeys;
	f SQLMoreResults;
	f SQLNativeSql;
	f SQLNumParams;
	f SQLParamOptions;
	f SQLPrimaryKeys;
	f SQLProcedureColumns;
	f SQLProcedures;
	f SQLSetPos;
	f SQLSetScrollOptions;
	f SQLTablePrivileges;
	f SQLBindParameter;
	f SQLBindParam;
	f SQLSetParam;
	f SQLGetDescField;
	f SQLSetDescField;
	f SQLGetDescRec;
	f SQLSetDescRec;
	f SQLGetDiagRec;
	f SQLGetDiagField;
	f SQLEndTran;
	f SQLFetchScroll;
	f SQLColAttribute;
	f SQLBulkOperations;
	f SQLCloseCursor;
	f SQLCopyDesc;
);

#define ODBCTraceStateVersion MAKELONG\
(\
	FIELD_OFFSET(struct ODBCTraceState, dwVersion),\
	FIELD_OFFSET(struct ODBCTraceState, fUnused) - FIELD_OFFSET(struct ODBCTraceState, dwVersion) - sizeof(DWORD)\
)

class __declspec(novtable) CTraceMsg
{
public:
	enum { MSGID = 1 };
	virtual void STDAPICALLTYPE Dump(HWND) = 0;
protected:
	void Send(SENDASYNCPROC = 0);
};

class TraceFmtV : public CTraceMsg
{
public:
	LPCTSTR fmt;
	va_list arglist;
	CHARFORMAT2 cf;
	TraceFmtV(COLORREF crTextColor, LPCTSTR fmt, va_list arglist, SENDASYNCPROC lpCallBack = 0)
	: fmt(fmt), arglist(arglist)
	{
		cf.cbSize = sizeof cf;
		cf.dwMask = CFM_COLOR;
		cf.crTextColor = crTextColor;
		cf.dwEffects = 0;
		Send(lpCallBack);
	}
	virtual void STDAPICALLTYPE Dump(HWND);
};

#ifdef ODBCTRACE_DLL_EXPORTS
#define ODBCTRACE_DLL __declspec(dllexport)
#define ODBCTRACE_DLL_DATA extern
#else
#define ODBCTRACE_DLL __declspec(dllimport)
#define ODBCTRACE_DLL_DATA typedef
#endif

ODBCTRACE_DLL_DATA struct ODBCTraceState ODBCTrace;

EXTERN_C struct ODBCTraceState ODBCTRACE_DLL *GetODBCTrace(DWORD dwVersion);

EXTERN_C BOOL ODBCTRACE_DLL CALLBACK ODBCTraceBootstrap(HWND, HINSTANCE, LPTSTR, int);

EXTERN_C void ODBCTRACE_DLL STDAPICALLTYPE ODBCTracePrintf(COLORREF, LPCTSTR, ...);
EXTERN_C void ODBCTRACE_DLL STDAPIVCALLTYPE ODBCTraceVPrintf(COLORREF, LPCTSTR, va_list);
