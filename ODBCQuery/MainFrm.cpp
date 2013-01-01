#include "stdafx.h"
#include "resource.h"
#include "ODBCQuery.h"
#include "DatabaseEx.h"
#include "DBRow.h"
#include "StyleDefinition.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_ENTERIDLE()
	ON_COMMAND(ID_FILE_REMOVE_RECENT_ITEMS, OnFileRemoveRecentItems)
	ON_UPDATE_COMMAND_UI(ID_FILE_REMOVE_RECENT_ITEMS, OnUpdateFileRemoveRecentItems)
	ON_COMMAND(ID_HELP_TRACE, OnHelpTrace)
	ON_UPDATE_COMMAND_UI(ID_HELP_TRACE, OnUpdateHelpTrace)
	ON_WM_COPYDATA()
	ON_WM_DROPFILES()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // Statusleistenanzeige
	ID_INDICATOR_OVR,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame Konstruktion/Zerstörung

CMainFrame::CMainFrame()
{
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	//TBSTYLE_FLAT
	if (!m_wndToolBar.Create(this, WS_CHILD | WS_VISIBLE |
		CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		return -1;
	}
	//m_wndToolBar.GetToolBarCtrl().SetExtendedStyle(TBSTYLE_EX_DRAWDDARROWS);
	m_wndToolBar.SendMessage(TB_SETEXTENDEDSTYLE, 0, TBSTYLE_EX_DRAWDDARROWS);

	TBBUTTONINFO tbi;
	tbi.dwMask = TBIF_STYLE;
	tbi.cbSize = sizeof(TBBUTTONINFO);
	//m_wndToolBar.GetToolBarCtrl().GetButtonInfo(ID_ZOOM_IN, &tbi);
	m_wndToolBar.SendMessage(TB_GETBUTTONINFO, ID_ZOOM_IN, reinterpret_cast<LPARAM>(&tbi));
	tbi.fsStyle |= TBSTYLE_DROPDOWN;
	//m_wndToolBar.GetToolBarCtrl().SetButtonInfo(ID_ZOOM_IN, &tbi);
	m_wndToolBar.SendMessage(TB_SETBUTTONINFO, ID_ZOOM_IN, reinterpret_cast<LPARAM>(&tbi));
	//m_wndToolBar.GetToolBarCtrl().GetButtonInfo(ID_ZOOM_OUT, &tbi);
	m_wndToolBar.SendMessage(TB_GETBUTTONINFO, ID_ZOOM_OUT, reinterpret_cast<LPARAM>(&tbi));
	tbi.fsStyle |= TBSTYLE_DROPDOWN;
	//m_wndToolBar.GetToolBarCtrl().SetButtonInfo(ID_ZOOM_OUT, &tbi);
	m_wndToolBar.SendMessage(TB_SETBUTTONINFO, ID_ZOOM_OUT, reinterpret_cast<LPARAM>(&tbi));

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators, RTL_NUMBER_OF(indicators)))
	{
		return -1;
	}
	DragAcceptFiles();
	return 0;
}

void CMainFrame::OnEnterIdle(UINT nWhy, CWnd* pWho) 
{
	CMDIFrameWnd::OnEnterIdle(nWhy, pWho);
	if (nWhy == MSGF_MENU)
	{
		int nID = 0;
		while (GetKeyState(VK_RBUTTON) < 0 || GetKeyState(VK_SPACE) < 0)
		{
			CWinApp *pApp = AfxGetApp();
			MSG msg;
			while (::PeekMessage(&msg, 0, 0, 0, PM_NOREMOVE))
			{
				pApp->PumpMessage();
			}
			nID = m_nIDTracking;
		}
		if (nID >= 0xE010 && nID <= 0xE01F ||
			nID >= ID_FILE_MRU_FIRST && nID <= ID_FILE_MRU_LAST)
		{
			CMenu *pMenu = GetMenu();
			UINT uState = pMenu->GetMenuState(nID, MF_BYCOMMAND);
			if ((uState & MF_GRAYED) == 0)
			{
				pMenu->CheckMenuItem(nID, uState  & MF_CHECKED ^ MF_CHECKED);
				RECT rect;
				pWho->GetClientRect(&rect);
				rect.right = GetSystemMetrics(SM_CXMENUCHECK);
				pWho->RedrawWindow(&rect);
			}
		}
	}
}

void CMainFrame::OnFileRemoveRecentItems() 
{
	CMenu *pMenu = GetMenu();
	UINT uChecked = 0;
	int i = 0, j = 0;
	// Work through recent connections menu (always present)
	int n = theApp.m_pDocTemplate->GetSize();
	while (i < n && (*theApp.m_pDocTemplate)[i].GetLength())
	{
		uChecked |= pMenu->GetMenuState(0xE010 + i, MF_BYCOMMAND);
		++i;
	}
	ASSERT(uChecked != 0xFFFFFFFF);
	if (MDIGetActive())
	{
		// Work through recent files menu (only present in context of active document)
		int n = theApp.m_pRecentFileList->GetSize();
		while (j < n && (*theApp.m_pRecentFileList)[j].GetLength())
		{
			uChecked |= pMenu->GetMenuState(ID_FILE_MRU_FIRST + j, MF_BYCOMMAND);
			++j;
		}
		ASSERT(uChecked != 0xFFFFFFFF);
	}
	// If none of the MRU items has a check mark, ask the user to confirm,
	// and then treat all items as checked.
	if ((uChecked & MF_CHECKED) == 0 &&
		AfxMessageBox(_T("Remove all recent items?"), MB_YESNO) == IDYES)
	{
		uChecked = MF_CHECKED;
	}
	else
	{
		uChecked = 0;
	}
	// Work backward through recent connections menu
	while (i)
	{
		--i;
		if ((pMenu->GetMenuState(0xE010 + i, MF_BYCOMMAND) | uChecked) & MF_CHECKED)
		{
			theApp.m_pDocTemplate->Remove(i);
			// Leave 1st item intact, but remove check mark
			if (i)
			{
				pMenu->DeleteMenu(0xE010 + i, MF_BYCOMMAND);
				if (pMenu->m_hMenu != m_hMenuDefault)
					::DeleteMenu(m_hMenuDefault, 0xE010 + i, MF_BYCOMMAND);
			}
			else
			{
				pMenu->CheckMenuItem(0xE010 + i, MF_BYCOMMAND);
				if (pMenu->m_hMenu != m_hMenuDefault)
					::CheckMenuItem(m_hMenuDefault, 0xE010 + i, MF_BYCOMMAND);
			}
		}
	}
	// Work backward through recent files menu
	while (j)
	{
		--j;
		if ((pMenu->GetMenuState(ID_FILE_MRU_FIRST + j, MF_BYCOMMAND) | uChecked) & MF_CHECKED)
		{
			theApp.m_pRecentFileList->Remove(j);
			// Leave 1st item intact, but remove check mark
			if (j)
			{
				pMenu->DeleteMenu(ID_FILE_MRU_FIRST + j, MF_BYCOMMAND);
				if (pMenu->m_hMenu != m_hMenuDefault)
					::DeleteMenu(m_hMenuDefault, ID_FILE_MRU_FIRST + j, MF_BYCOMMAND);
			}
			else
			{
				pMenu->CheckMenuItem(ID_FILE_MRU_FIRST + j, MF_BYCOMMAND);
				if (pMenu->m_hMenu != m_hMenuDefault)
					::CheckMenuItem(m_hMenuDefault, ID_FILE_MRU_FIRST + j, MF_BYCOMMAND);
			}
		}
	}
}

void CMainFrame::OnUpdateFileRemoveRecentItems(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(
		(*theApp.m_pDocTemplate)[0].GetLength()
	||	MDIGetActive() && (*theApp.m_pRecentFileList)[0].GetLength());
}

void CMainFrame::OnHelpTrace() 
{
	BOOL bTrace = FALSE;
	::SQLGetConnectOption(0, SQL_OPT_TRACE, &bTrace);
	bTrace = !bTrace;
	if (bTrace)
	{
		// If ODBC has been configured to use a custom tracer, let it happen.
		// Otherwise, hook the ODBCTrace.dll into ODBC's default tracer.
		static const TCHAR szKey[] = _T("Software\\ODBC\\ODBC.INI\\ODBC");
		static const TCHAR szValue[] = _T("TraceDll");
		TCHAR szData[MAX_PATH];
		DWORD dwType = REG_NONE;
		DWORD cbData = sizeof szData;
		if (::SHRegGetUSValue(szKey, szValue, &dwType, szData, &cbData, FALSE, 0, 0) != 0 ||
			dwType == REG_SZ && ::PathMatchSpec(szData, _T("*\\odbctrac.dll")))
		{
			static BOOL bHooked = FALSE;
			if (HMODULE hModule = ::LoadLibrary(_T("ODBCTrace.dll")))
				if (FARPROC pfn = ::GetProcAddress(hModule, "ODBCTraceHookIntoSystemTracer"))
					if (pfn())
						bHooked = TRUE;
					else
						::FreeLibrary(hModule);
			bTrace = bHooked;
		}
	}
	::SQLSetConnectOption(0, SQL_OPT_TRACE, bTrace);
}

void CMainFrame::OnUpdateHelpTrace(CCmdUI* pCmdUI) 
{
	BOOL bTrace = FALSE;
	::SQLGetConnectOption(0, SQL_OPT_TRACE, &bTrace);
	pCmdUI->SetCheck(bTrace);
}

TCHAR CMainFrame::m_szClassName[40];

LPCTSTR CMainFrame::RegisterWndClass()
{
	TCHAR szPath[MAX_PATH];
	::GetModuleFileName(0, szPath, MAX_PATH);
	HANDLE handle = ::CreateFile(szPath, FILE_READ_ATTRIBUTES,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	BY_HANDLE_FILE_INFORMATION fi;
	// Make class name unique with respect to the executable's location on disk
	wsprintf(m_szClassName, ::GetFileInformationByHandle(handle, &fi)
		? _T("ODBCQuery:%08lX:%08lX:%08lX") : _T("ODBCQuery:Global"),
		fi.dwVolumeSerialNumber, fi.nFileIndexHigh, fi.nFileIndexLow);
	::CloseHandle(handle);
	// Register the class
	WNDCLASS wndcls;
	wndcls.style = CS_DBLCLKS;
	wndcls.lpfnWndProc = ::DefWindowProc;
	wndcls.cbClsExtra = wndcls.cbWndExtra = 0;
	wndcls.hInstance = theApp.m_hInstance;
	wndcls.hIcon = theApp.LoadIcon(IDR_MAINFRAME);
	wndcls.hCursor = ::LoadCursor(0, IDC_ARROW);
	wndcls.hbrBackground = 0;
	wndcls.lpszMenuName = 0;
	wndcls.lpszClassName = m_szClassName;
	if (!AfxRegisterClass(&wndcls))
		AfxThrowResourceException();
	return m_szClassName;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.lpszClass = m_szClassName;
	return CMDIFrameWnd::PreCreateWindow(cs);
}

BOOL CMainFrame::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	if (BSTR bstrData = ::SysAllocStringByteLen(
		(LPCSTR)pCopyDataStruct->lpData, pCopyDataStruct->cbData))
	{
		SetForegroundWindow();
		theApp.m_pDocTemplate->ParseCommandLine((LPTSTR)bstrData);
		::SysFreeString(bstrData);
	}
	return CMDIFrameWnd::OnCopyData(pWnd, pCopyDataStruct);
}

void CMainFrame::OnDropFiles(HDROP h)
{
	UINT i = DragQueryFile(h, 0xFFFFFFFF, 0, 0);
	UINT n = 0;
	while (i)
	{
		n += DragQueryFile(h, --i, 0, 0) + 3;
	}
	CString s;
	LPTSTR p = s.GetBufferSetLength(n);
	LPTSTR q = p;
	while (n)
	{
		LPTSTR p = q;
		*q++ = '"';
		q += DragQueryFile(h, i++, q, n);
		*q++ = '"';
		*q++ = '\n';
		n -= q - p;
	}
	q[-1] = '\0';
	DragFinish(h);
	theApp.m_pDocTemplate->ParseCommandLine(p);
}
