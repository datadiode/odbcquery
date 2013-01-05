#include "stdafx.h"
#include "resource.h"
#include "helpers.h"
#include "DBRow.h"
#include "DatabaseEx.h"
#include "DBInfo.h"
#include "ODBCQuery.h"
#include "ODBCQueryDoc.h"
#include "ChildFrm.h"
#include "GenSQLDlg.h"
#include "InputBox.h"
#include "TextBox.h"
#include "HexBox.h"
#include "CellEditor.h"
#include "ExcelExport.h"

#include <oleacc.h>
#pragma comment(lib, "oleacc.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static CWnd *NTAPI ShowCurrentPage(HWND hTc, int nCmdShow = SW_SHOWNORMAL)
{
	CWnd *pwndPage = 0;
	if (hTc)
	{
		int nPage = TabCtrl_GetCurSel(hTc);
		if (nPage != -1)
		{
			TCITEM tci;
			tci.mask = TCIF_PARAM;
			TabCtrl_GetItem(hTc, nPage, &tci);
			RECT r;
			::GetClientRect(hTc, &r);
			TabCtrl_AdjustRect(hTc, FALSE, &r);
			::MapWindowPoints(hTc, ::GetParent(hTc), (LPPOINT)&r, 2);
			pwndPage = reinterpret_cast<CWnd *>(tci.lParam);
			pwndPage->MoveWindow(&r, TRUE);
			::ShowWindow(pwndPage->m_hWnd, nCmdShow);
			if (nCmdShow == SW_HIDE)
				pwndPage = 0;
		}
	}
	return pwndPage;
}

static BOOL WINAPI ConsoleCtrlHandler(DWORD dwCtrlType)
{
	switch (dwCtrlType)
	{
	case CTRL_C_EVENT:
	case CTRL_BREAK_EVENT:
		return TRUE;
	}
	return FALSE;
}

static BOOL NTAPI DisallowInPlaceEdit(CODBCFieldInfo &fieldInfo)
{
	BOOL f = FALSE;
	switch (fieldInfo.m_nSQLType)
	{
	case SQL_LONGVARCHAR:
	case SQL_WLONGVARCHAR:
	case SQL_LONGVARBINARY:
		f = TRUE;
		break;
	case SQL_CHAR:
	case SQL_VARCHAR:
	case SQL_WCHAR:
	case SQL_WVARCHAR:
		if (fieldInfo.m_nPrecision > 2048)
			f = TRUE;
		break;
	case SQL_BINARY:
	case SQL_VARBINARY:
		if (fieldInfo.m_nPrecision > 1024)
			f = TRUE;
		break;
	}
	return f;
}

/////////////////////////////////////////////////////////////////////////////
// CChildFrame

ATOM CChildFrame::m_cwTabCtrl = 0;
ATOM CChildFrame::m_cwComboBox = 0;

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CChildFrame)
	ON_WM_ERASEBKGND()
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_DESTROY()
	ON_WM_PARENTNOTIFY()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_QUERY_NEW, OnQueryNew)
	ON_WM_ENTERIDLE()
	ON_WM_MENUCHAR()
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_WM_NCACTIVATE()
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR, OnUpdateEditClear)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, OnUpdateEditSelectAll)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_COMMAND(ID_EDIT_COPY_HTML, OnEditCopyHtml)
	ON_COMMAND(ID_ZOOM_IN, OnZoomIn)
	ON_COMMAND(ID_ZOOM_OUT, OnZoomOut)
	ON_COMMAND(ID_ZOOM_ZERO, OnZoomZero)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_IN, OnUpdateZoom)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_ZERO, OnUpdateZoomZero)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_OVR, OnUpdateOvrIndicator)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY_HTML, OnUpdateEditCopy)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_OUT, OnUpdateZoom)
	//}}AFX_MSG_MAP
	ON_NOTIFY_RANGE(NM_CUSTOMDRAW, 2000, 2019, OnCustomdrawRangeLv)
	ON_NOTIFY_RANGE(LVN_DELETEITEM, 2000, 2019, OnDBRowDeleteitemRangeLv)
	ON_NOTIFY_RANGE(LVN_GETDISPINFOW, 2000, 2019, OnDBRowGetdispinfoRangeLv)
	ON_NOTIFY_RANGE(LVN_COLUMNCLICK, 2000, 2019, OnDBRowSortcolumnRangeLv)
	ON_NOTIFY_RANGE(LVN_KEYDOWN, 2000, 2019, OnDBRowKeydownRangeLv)
	ON_NOTIFY_RANGE(NM_SETFOCUS, 2000, 2000, OnDBRowSetfocusRangeLv)
	ON_NOTIFY_RANGE(LVN_ITEMCHANGING, 2000, 2000, OnDBRowItemchangingRangeLv)
	ON_NOTIFY_RANGE(LVN_ITEMCHANGED, 2000, 2000, OnDBRowItemchangedRangeLv)
	ON_CONTROL_RANGE(EN_SETFOCUS, 2000, 2000, OnDBRowEditSetfocus)
	ON_CONTROL_RANGE(EN_KILLFOCUS, 2000, 2000, OnDBRowEditKillfocus)
	ON_NOTIFY_RANGE(LVN_ITEMACTIVATE, 2000, 2000, OnDBRowActivateRangeLv)
	ON_NOTIFY_RANGE(NM_CLICK, 2000, 2000, OnDBRowClickRangeLv)
	ON_NOTIFY_RANGE(TCN_SELCHANGING, 101, 103, OnSelchangeRangeTc)
	ON_NOTIFY_RANGE(TCN_SELCHANGE, 101, 103, OnSelchangeRangeTc)
	ON_NOTIFY_RANGE(LVN_COLUMNCLICK, 1031, 1039, OnSortcolumnRangeLv)
	ON_NOTIFY_RANGE(LVN_KEYDOWN, 1031, 1039, OnKeydownRangeLv)
	ON_NOTIFY(NM_DBLCLK, 1031, OnDblclkLvTables)
	ON_NOTIFY(NM_RETURN, 1031, OnDblclkLvTables)
	ON_NOTIFY(NM_DBLCLK, 1032, OnDblclkLvProcedures)
	ON_NOTIFY(NM_RETURN, 1032, OnDblclkLvProcedures)
	ON_NOTIFY(TBN_DROPDOWN, AFX_IDW_TOOLBAR, OnDropdownTb)
	ON_CBN_SELCHANGE(104, OnSelchangeCbUse)
	ON_MESSAGE_VOID(WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CChildFrame, CMDIChildWnd)
	DISP_FUNCTION(CChildFrame, "Sql", AppendSql, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CChildFrame, "Log", AppendLogStyled, VT_EMPTY, VTS_BSTR)
	DISP_PROPERTY_PARAM(CChildFrame, "LogStyle", GetLogStyle, SetLogStyle, VT_BSTR, VTS_I4)
	DISP_FUNCTION(CChildFrame, "Clear", ClearResults, VT_EMPTY, VTS_NONE)
	DISP_PROPERTY_EX(CChildFrame, "rdoConnxn", rdoConnxn, SetNotSupported, VT_DISPATCH)
	DISP_PROPERTY_EX(CChildFrame, "rdoEngine", rdoEngine, SetNotSupported, VT_DISPATCH)
	DISP_FUNCTION(CChildFrame, "ShowConsole", ShowConsole, VT_DISPATCH, VTS_I4)
	DISP_FUNCTION(CChildFrame, "FreeConsole", FreeConsole, VT_BOOL, VTS_NONE)
END_DISPATCH_MAP()

BEGIN_EVENTSINK_MAP(CChildFrame, CMDIChildWnd)
	ON_EVENT(CChildFrame, 105, 102 /* StatusTextChange */, OnStatusTextChange, VTS_BSTR)
END_EVENTSINK_MAP()

BEGIN_OLECMD_MAP(CChildFrame, CMDIChildWnd)
END_OLECMD_MAP()

CChildFrame::CChildFrame()
{
	static const long FloatScript[] =
	{
		101,			BY<200>::X2L | BY<1000>::X2R | BY<500>::Y2B,
		102,			BY<200>::X2L | BY<1000>::X2R | BY<500>::Y2T | BY<1000>::Y2B,
		103,			BY<200>::X2R | BY<1000>::Y2B,
		104,			BY<200>::X2R,
		0
	};
	static const long SplitScript[] =
	{
		101, 102, ~20,// ~40,
		102, 104, ~20,// ~40,
		0
	};
	CFloatState::FloatScript = FloatScript;
	CSplitState::SplitScript = SplitScript;
	EnableAutomation();
	m_pdispCmdTarget = GetIDispatch(FALSE);
	m_iLogStyle = 0;
}

CChildFrame::~CChildFrame()
{
}

void CChildFrame::OnUpdateFrameTitle(BOOL bAddToTitle)
{
	CODBCQueryDoc *pDocument = static_cast<CODBCQueryDoc *>(GetActiveDocument());
	if (pDocument == 0)
		return;
	CString strTitle;
	char szDSN[200];
	SWORD cbData = 0;
	if (HDBC hdbc = pDocument->m_db.m_hdbc)
	{
		::SQLGetInfo(hdbc, SQL_DATA_SOURCE_NAME, szDSN, sizeof szDSN, &cbData);
		if (cbData == 0)
			::SQLGetInfo(hdbc, SQL_DBMS_NAME, szDSN, sizeof szDSN, &cbData);
	}
	if (cbData == 0)
		lstrcpyA(szDSN, "n/c");
	strTitle.Format(_T("%hs - %s"), szDSN, pDocument->GetTitle());
	if (pDocument->GetPathName().IsEmpty())
		strTitle += _T(" - (New Query)");
	SetWindowText(strTitle);
}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	CMDIChildWnd::PreCreateWindow(cs);
	cs.style |= WS_CLIPCHILDREN;
	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	return TRUE;
}

BOOL CChildFrame::OnEraseBkgnd(CDC *pDC) 
{
	pDC->SetBkColor(GetSysColor(COLOR_BTNFACE));
	RECT r;
	GetClientRect(&r);
	pDC->ExtTextOut(0, 0, ETO_OPAQUE, &r, 0, 0, 0);
	return TRUE;
}

void CChildFrame::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	lpwndpos->flags &= ~SWP_NOCOPYBITS;
	CFloatState::Float(lpwndpos);
	CMDIChildWnd::OnWindowPosChanging(lpwndpos);
}

BOOL CChildFrame::PreTranslateMessage(MSG *pMsg)
{
	switch (pMsg->message)
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MOUSEMOVE:
		if (pMsg->hwnd == m_hWnd)
			if (CSplitState::Split(m_hWnd) == 2)
				RecalcLayout();
		break;
	case WM_SYSKEYDOWN:
		switch (pMsg->wParam)
		{
		case VK_LEFT: case VK_RIGHT: case VK_HOME: case VK_END:
			{
				HWND hwndNext = pMsg->hwnd;
				if (::GetClassWord(hwndNext, GCW_ATOM) == m_cwTabCtrl ||
					::GetClassWord(hwndNext = ::GetNextDlgTabItem(m_hWnd,
						hwndNext, FALSE), GCW_ATOM) == m_cwTabCtrl)
				{
					pMsg->hwnd = hwndNext;
					pMsg->message = WM_KEYDOWN;
				}
			}
			break;
		case VK_UP: case VK_DOWN:
			{
				HWND hwndNext = pMsg->hwnd;
				WORD cwNext = ::GetClassWord(hwndNext, GCW_ATOM);
				if (cwNext == m_cwTabCtrl ||
					cwNext != m_cwComboBox && 
					::GetClassWord(hwndNext = ::GetNextDlgTabItem(m_hWnd,
						hwndNext, FALSE), GCW_ATOM) == m_cwTabCtrl)
				{
					if (hwndNext == pMsg->hwnd)
						hwndNext = ::GetNextDlgGroupItem(m_hWnd, hwndNext, TRUE);
					::SetFocus(hwndNext);
					return TRUE;
				}
			}
			break;
		}
		break;
	case WM_KEYDOWN:
		switch (pMsg->wParam)
		{
		case VK_TAB:
			if (CScintillaCtrl *pCtrl = DYNAMIC_DOWNCAST(CScintillaCtrl, GetFocus()))
				if (!pCtrl->GetReadOnly())
					return CMDIChildWnd::PreTranslateMessage(pMsg);
			break;
		case VK_F4:
			if (GetKeyState(VK_CONTROL) >= 0)
			{
				HWND hwndNext = pMsg->hwnd;
				WORD cwNext = ::GetClassWord(hwndNext, GCW_ATOM);
				if (cwNext == m_cwTabCtrl ||
					cwNext != m_cwComboBox && 
					::GetClassWord(hwndNext = ::GetNextDlgTabItem(m_hWnd,
						hwndNext, FALSE), GCW_ATOM) == m_cwTabCtrl)
				{
					if (hwndNext == pMsg->hwnd)
						hwndNext = ::GetNextDlgGroupItem(m_hWnd, hwndNext, TRUE);
					::SetFocus(hwndNext);
					return TRUE;
				}
			}
			break;
		case VK_F6:
			{
				BOOL bPrevious = ::GetKeyState(VK_SHIFT) < 0;
				HWND hwndNext = pMsg->hwnd;
				if ((::GetWindowLong(hwndNext, GWL_STYLE) & WS_TABSTOP) == 0)
					hwndNext = ::GetNextDlgTabItem(m_hWnd, hwndNext, FALSE);	
				hwndNext = ::GetNextDlgTabItem(m_hWnd, hwndNext, bPrevious);
				if (::GetClassWord(hwndNext, GCW_ATOM) == m_cwTabCtrl)
					hwndNext = ::GetNextDlgGroupItem(m_hWnd, hwndNext, TRUE);
				::SetFocus(hwndNext);
				return TRUE;
			}
			break;
		case VK_RETURN:
			if ((::SendMessage(pMsg->hwnd, WM_GETDLGCODE, 0, 0) & DLGC_HASSETSEL) == 0)
			{
				NMHDR nm;
				nm.hwndFrom = pMsg->hwnd;
				nm.idFrom = ::GetDlgCtrlID(nm.hwndFrom);
				nm.code = NM_RETURN;
				::SendMessage(::GetParent(nm.hwndFrom), WM_NOTIFY, nm.idFrom, (LPARAM)&nm);
				return TRUE;
			}
			break;
		}
		if (GetKeyState(VK_CONTROL) < 0 ||
			pMsg->wParam >= VK_F1 && pMsg->wParam <= VK_F24)
		{
			if (pMsg->wParam == VK_CANCEL)
			{
				GetDocument()->CancelQuery();
				return TRUE;
			}
			return CMDIChildWnd::PreTranslateMessage(pMsg) ||
				CWebBrowserHost::PreTranslateMessage(pMsg);
		}
		break;
	}
	return PreTranslateInput(pMsg) ||
		CWebBrowserHost::PreTranslateMessage(pMsg);
}

static CString CollectKeys(CMapStringToPtr &rg, LPCTSTR szDelimiter)
{
	CString keys;
	POSITION pos = rg.GetStartPosition();
	bool bExclusions = false;
	while (pos)
	{
		CString key;
		UINT_PTR value;
		rg.GetNextAssoc(pos, key, reinterpret_cast<void *&>(value));
		if (value & 0x80000000)
			keys += &szDelimiter[keys.GetLength() == 0] + key + &szDelimiter[TRUE];
		else
			bExclusions = true;
	}
	if (!bExclusions)
		keys.Empty();
	return keys;
}

void CChildFrame::PopulateLvTables()
{
	CString strOwners = CollectKeys(m_rgOwnerTables, _T(","));
	if (strOwners.Find(',') != -1)
		strOwners.Empty();
	CString strTypes = CollectKeys(m_rgTypeTables, _T(",'"));
	CRecordsetEx *prs = CRecordsetEx::New(GetDatabase());
	CString sName, sOwner, sType;
	CLvPopulator populator = m_LvTables.m_hWnd;
	try
	{
		prs->m_strSQL.Format(_T(";[%s];;[%s];"), strOwners, strTypes);
		prs->m_lOpen = SQL_API_SQLTABLES;
		prs->Execute();
		while (!prs->IsEOF())
		{
			prs->GetFieldValue(1, sOwner);
			prs->GetFieldValue(2, sName);
			prs->GetFieldValue(3, sType);
			populator->Append(sName)->Append(sOwner)->Append(sType);
			reinterpret_cast<UINT_PTR &>(m_rgOwnerTables[sOwner]) |= 0x80000000;
			reinterpret_cast<UINT_PTR &>(m_rgTypeTables[sType]) |= 0x80000000;
			prs->MoveNext();			
		}
	}
	catch (CException *e)
	{
		AppendLog(e, MB_ICONSTOP);
	}
	prs->Release();
}

void CChildFrame::PopulateLvProcedures()
{
	CString strOwners = CollectKeys(m_rgOwnerProcedures, _T(","));
	if (strOwners.Find(',') != -1)
		strOwners.Empty();
	CRecordsetEx *prs = CRecordsetEx::New(GetDatabase());
	CString sName, sOwner;
	CLvPopulator populator = m_LvProcedures.m_hWnd;
	try
	{
		prs->m_strSQL.Format(_T(";[%s];;"), strOwners);
		prs->m_lOpen = SQL_API_SQLPROCEDURES;
		prs->Execute();
		while (!prs->IsEOF())
		{
			prs->GetFieldValue(1, sOwner);
			prs->GetFieldValue(2, sName);
			sName.ReleaseBuffer(sName.Find(';'));
			populator->Append(sName)->Append(sOwner);
			reinterpret_cast<UINT_PTR &>(m_rgOwnerProcedures[sOwner]) |= 0x80000000;
			prs->MoveNext();			
		}
	}
	catch (CException *e)
	{
		AppendLog(e, MB_ICONSTOP);
	}
	prs->Release();
}

void CChildFrame::PopulateTcDictionary()
{
	m_rgOwnerTables.RemoveAll();
	m_rgTypeTables.RemoveAll();
	m_rgOwnerProcedures.RemoveAll();

	m_LvTables.EnableWindow(FALSE);
	m_LvProcedures.EnableWindow(FALSE);

	if (CWnd *pwndPage = ::ShowCurrentPage(m_TcDictionary.m_hWnd))
		PopulatePage(pwndPage);
}

void CChildFrame::OnSelchangeCbUse()
{
	CWaitCursor wait;
	CString sUse;
	try
	{
		m_CbUse.GetWindowText(sUse);
		GetDatabase()->ExecuteSQL(_T("USE \"") + sUse + _T("\""));
	}
	catch (CException *e)
	{
		AppendLog(e, MB_ICONSTOP);
		PopulateCbUse();
	}
	// Forget about column widths
	m_LvTables.DeleteAllItems();
	m_LvProcedures.DeleteAllItems();
	// Populate the list views
	PopulateTcDictionary();
}

void CChildFrame::OnInitialUpdate()
{
	CWaitCursor wait;
	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_TEXT;
	lvc.fmt = LVCFMT_LEFT;
	lvc.pszText = _T("Name\0Owner\0Type\0");
	CLvPopulator::InsertColumns(m_LvTables.m_hWnd, &lvc);
	lvc.pszText = _T("Name\0Owner\0");
	CLvPopulator::InsertColumns(m_LvProcedures.m_hWnd, &lvc);
	PopulateCbUse();
	PopulateTcDictionary();
}

int CChildFrame::AppendLog(CException *e, UINT type)
{
	int response = 0;
	TCHAR szText[1024];
	if (CDBException *dbe = DYNAMIC_DOWNCAST(CDBException, e))
		m_strStatus = dbe->m_strError + dbe->m_strStateNativeOrigin;
	else if (e->GetErrorMessage(szText, RTL_NUMBER_OF(szText)))
		m_strStatus = szText;
	else
		m_strStatus = e->GetRuntimeClass()->m_lpszClassName;
	e->Delete();
	AppendLog(m_strStatus);
	if (type)
		response = AfxMessageBox(m_strStatus, type);
	return response;
}

void CChildFrame::PopulateCbUse()
{
	m_CbUse.ResetContent();
	CRecordset rs = GetDatabase();
	CString sAdd, sCur;
	try
	{ 
		static const TCHAR szSQL[] = _T("SELECT NAME, DB_NAME() FROM MASTER..SYSDATABASES");
		rs.Open(CRecordset::forwardOnly, szSQL, CRecordset::executeDirect);
		while (!rs.IsEOF())
		{
			rs.GetFieldValue((short)0, sAdd);
			rs.GetFieldValue((short)1, sCur);
			int i = m_CbUse.AddString(sAdd);
			if (sAdd == sCur)
				m_CbUse.SetCurSel(i);
			rs.MoveNext();
		}
	}
	catch (CException *e)
	{
		AppendLog(e);
		m_CbUse.EnableWindow(FALSE);
		m_CbUse.SetCurSel(m_CbUse.AddString(_T("Catalog selection is not supported")));
	}
}

void CChildFrame::LoadStyles(CScintillaCtrl &rCtrl)
{
	rCtrl.SetTabWidth(4);
	rCtrl.SetScrollWidth(20000);
	rCtrl.SetLexer(SCLEX_SQL);
	CString strKeywords(SQL_ODBC_KEYWORDS);
	strKeywords.Replace(',', ' ');
	strKeywords.MakeLower();
	rCtrl.SetKeyWords(0, strKeywords);

	CStyleDefinition rgsd[STYLE_MAX + 1];
	TCHAR szPath[MAX_PATH];
	::GetModuleFileName(0, szPath, MAX_PATH);
	::PathRenameExtension(szPath, _T(".ini"));
	rgsd->LoadStyles(szPath, _T("sql.styles"));
	rgsd->SetStyles(rCtrl);
}

BOOL CChildFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext *pContext)
{
	m_hWndFocus = 0;
	RECT r, rectWindow;
	GetWindowRect(&rectWindow);
	GetParent()->ScreenToClient(&rectWindow);
	SetWindowPos(0, 0, 0, 100, 100, SWP_NOSENDCHANGING|SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE);

	GetClientRect(&r);

	r.right = r.right / 5 - 2;

	RECT rectCombo = r;
	rectCombo.bottom = GetSystemMetrics(SM_CXSCREEN) / 2;
	m_CbUse.Create(WS_CHILD|WS_VISIBLE|WS_TABSTOP|CBS_DROPDOWNLIST|CBS_SORT, rectCombo, this, 104);
	m_cwComboBox = ::GetClassWord(m_CbUse.m_hWnd, GCW_ATOM);
	m_CbUse.GetWindowRect(&rectCombo);
	r.top += rectCombo.bottom - rectCombo.top;

	m_LvTables.Create(WS_DISABLED|WS_CHILD|WS_BORDER|LVS_REPORT|LVS_SHOWSELALWAYS, r, this, 1031);
	m_LvTables.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);

	m_LvProcedures.Create(WS_DISABLED|WS_CHILD|WS_BORDER|LVS_REPORT|LVS_SHOWSELALWAYS, r, this, 1032);
	m_LvProcedures.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);

	m_TcDictionary.Create(WS_CHILD|WS_BORDER|WS_VISIBLE|WS_TABSTOP, r, this, 103);
	m_cwTabCtrl = ::GetClassWord(m_TcDictionary.m_hWnd, GCW_ATOM);

	TCITEM tci;
	tci.mask = TCIF_TEXT | TCIF_PARAM;
	tci.cchTextMax = 0;
	tci.pszText = _T("Tables");
	tci.lParam = reinterpret_cast<LPARAM>(&m_LvTables);
	m_TcDictionary.InsertItem(0, &tci);
	tci.pszText = _T("Procedures");
	tci.lParam = reinterpret_cast<LPARAM>(&m_LvProcedures);
	m_TcDictionary.InsertItem(1, &tci);

	GetClientRect(&r);
	r.left = r.right / 5 + 2;
	r.bottom = r.bottom / 2 - 2;

	CScintillaView *pView = CODBCQueryDoc::NewScintillaView();
	if (!pView->Create(0, 0, WS_CHILD|WS_BORDER, r, this, 106, pContext))
	{
		delete pView;
		return FALSE; // can't continue without a view
	}

	if (!InitControlContainer())
		return FALSE;

	// Create the WebBrowser control window
	if (!CWebBrowserHost::CreateControl(m_pCtrlCont, &m_wndBrowser,
		WS_CHILD|WS_DISABLED, r, 105))
	{
		return FALSE;
	}
	LPUNKNOWN lpUnk = m_wndBrowser.GetControlUnknown();
	if (FAILED(lpUnk->QueryInterface(&m_pBrowserApp)))
	{
		m_pBrowserApp = 0;
		return FALSE;
	}

	m_TcScripts.Create(WS_CHILD|WS_BORDER|WS_VISIBLE|WS_TABSTOP, r, this, 101);
	tci.pszText = _T("SQL");
	tci.lParam = reinterpret_cast<LPARAM>(pView);
	m_TcScripts.InsertItem(0, &tci);
	tci.pszText = _T("HTML");
	tci.lParam = reinterpret_cast<LPARAM>(&m_wndBrowser);
	m_TcScripts.InsertItem(1, &tci);
	// Preserve the focus to give its owner a chance to save it and return to it
	// upon reactivation
	HWND hwndFocus = ::GetFocus();
	SetActiveView(pView);
	::SetFocus(hwndFocus);

	pView->GetCtrl().ModifyStyle(WS_TABSTOP, 0);
	LoadStyles(pView->GetCtrl());

	GetClientRect(&r);

	r.left = r.right / 5 + 2;
	r.top = r.bottom / 2 + 2;

	m_EdLog.Create(WS_CHILD|WS_BORDER, r, this, 1000);
	m_EdLog.StyleClearAll();
	m_EdLog.SetReadOnly(TRUE);

	m_TcResults.Create(WS_CHILD|WS_BORDER|WS_VISIBLE|WS_TABSTOP|TCS_FORCEICONLEFT, r, this, 102);
	m_TcResults.SetImageList(&GetDocTemplate()->m_imagelist);

	tci.pszText = _T("Log");
	tci.lParam = reinterpret_cast<LPARAM>(&m_EdLog);
	m_TcResults.InsertItem(0, &tci);

	m_TcResults.GetClientRect(&r);
	m_TcResults.AdjustRect(FALSE, &r);
	m_TcResults.MapWindowPoints(this, &r);

	CFloatState::Clear();
	MoveWindow(&rectWindow);

	return TRUE;
}

void CChildFrame::RecalcLayout(BOOL)
{
	::ShowCurrentPage(m_TcScripts.m_hWnd);
	::ShowCurrentPage(m_TcResults.m_hWnd);
	::ShowCurrentPage(m_TcDictionary.m_hWnd);
}

/////////////////////////////////////////////////////////////////////////////
// CResultView message handlers

void CChildFrame::OnCustomdrawRangeLv(UINT, NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVCUSTOMDRAW *pNM = (NMLVCUSTOMDRAW *)pNMHDR;
	RECT rc, rc2;
	switch (pNM->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT:
		// Cosmetic: As the top grid line is outside the item rectangle,
		// CDDS_ITEMPOSTPAINT | CDDS_SUBITEM is likely to clip it away.
		// The fix is to draw the top-most grid line at CDDS_PREPAINT.
		if (HWND hHd = ListView_GetHeader(pNMHDR->hwndFrom))
		{
			WINDOWPLACEMENT wp;
			wp.length = sizeof wp;
			::GetWindowPlacement(hHd, &wp);
			Header_GetItemRect(hHd, Header_GetItemCount(hHd) - 1, &rc);
			wp.rcNormalPosition.right = wp.rcNormalPosition.left + rc.right;
			wp.rcNormalPosition.top = wp.rcNormalPosition.bottom + 1;
			wp.rcNormalPosition.bottom = wp.rcNormalPosition.top + 1;
			if (long count = ListView_GetItemCount(pNMHDR->hwndFrom))
			{
				ListView_GetItemRect(pNMHDR->hwndFrom, count - 1, &rc, LVIR_BOUNDS);
				wp.rcNormalPosition.bottom = rc.bottom;
			}
			::DrawEdge(pNM->nmcd.hdc, &wp.rcNormalPosition, EDGE_RAISED, BF_LEFT);
			++wp.rcNormalPosition.left;
			::DrawEdge(pNM->nmcd.hdc, &wp.rcNormalPosition, EDGE_RAISED, BF_TOPLEFT);
		}
		*pResult = CDRF_NOTIFYITEMDRAW | CDRF_NOTIFYPOSTPAINT;
		break;
	case CDDS_POSTPAINT:
		break;
	case CDDS_ITEMPREPAINT:
		*pResult = CDRF_NOTIFYSUBITEMDRAW;
		if (pNM->nmcd.uItemState & CDIS_FOCUS)
		{
			pNM->nmcd.uItemState &= ~CDIS_FOCUS;
			// Draw a focus rectangle only if there is no cell editor
			if (::FindWindowEx(pNMHDR->hwndFrom, 0, CellEditor_EditClassName, 0) == 0)
				*pResult |= CDRF_NOTIFYPOSTPAINT;
		}
		break;
	case CDDS_ITEMPOSTPAINT:
		// Draw a focus rectangle
		ListView_GetItemRect(pNMHDR->hwndFrom, pNM->nmcd.dwItemSpec, &rc, LVIR_BOUNDS);
		ListView_GetItemRect(pNMHDR->hwndFrom, pNM->nmcd.dwItemSpec, &rc2, LVIR_LABEL);
		--rc.right;
		--rc.bottom;
		rc.left = rc2.left;
		::DrawFocusRect(pNM->nmcd.hdc, &rc);
		break;
	case CDDS_ITEMPREPAINT | CDDS_SUBITEM:
		*pResult = CDRF_NOTIFYPOSTPAINT;
		break;
	case CDDS_ITEMPOSTPAINT | CDDS_SUBITEM:
		ListView_GetSubItemRect(pNMHDR->hwndFrom, pNM->nmcd.dwItemSpec, pNM->iSubItem, LVIR_LABEL, &rc);
		++rc.right;
		++rc.bottom;
		::DrawEdge(pNM->nmcd.hdc, &rc, EDGE_SUNKEN, BF_BOTTOMRIGHT);
	}
}

void CChildFrame::OnDBRowDeleteitemRangeLv(UINT, NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLISTVIEW *pNM = (NMLISTVIEW *)pNMHDR;
	delete reinterpret_cast<CObject *>(pNM->lParam);
}

void CChildFrame::OnDBRowGetdispinfoRangeLv(UINT, NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pNM = (NMLVDISPINFO *)pNMHDR;
	if (CDBRow::Handle *pRow = reinterpret_cast<CDBRow::Handle *>(pNM->item.lParam))
	{
		CODBCFieldInfo *rgODBCFieldInfos = (CODBCFieldInfo *)::GetWindowLong(pNM->hdr.hwndFrom, GWL_USERDATA);
		CODBCFieldInfo &fieldInfo = rgODBCFieldInfos[pNM->item.iSubItem];
		CString s = pRow->Item(pNM->item.iSubItem)->asString(0,
			DisallowInPlaceEdit(fieldInfo) ? CDBRow::Item::fTruncate : 0);
		if (int n = s.GetLength())
		{
			if (s.GetAt(n - 1))
			{
				// Looks like an ANSI string
				CRecordsetEx::FixScale(fieldInfo, s);
				n = s.GetLength();
				// Convert to Unicode
				CString u;
				int cch = MultiByteToWideChar(CP_ACP, 0, s, n, 0, 0);
				LPWSTR pch = (LPWSTR)u.GetBufferSetLength(cch * sizeof(WCHAR) + 1);
				MultiByteToWideChar(CP_ACP, 0, s, n, pch, cch);
				pch[cch] = '\0';
				s = u;
			}
			else
			{
				ASSERT(n & 1);
			}
			pNM->item.mask = LVIF_TEXT;
			pNM->item.pszText = CHelpers::AllocDispinfoText(s);
		}
	}
}

void CChildFrame::OnDBRowSortcolumnRangeLv(UINT, NMHDR *pNMHDR, LRESULT *pResult)
{
	CWaitCursor wait;
	CSortListCtrl::From(pNMHDR)->Sort(CDBRow::Handle::LVCompare);
}

void CChildFrame::OnDBRowKeydownRangeLv(UINT, NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVKEYDOWN *pParam = (NMLVKEYDOWN *)pNMHDR;
	CListCtrl *pLv = static_cast<CListCtrl *>(CWnd::FromHandlePermanent(pNMHDR->hwndFrom));
	switch (pParam->wVKey)
	{
	case 'A':
		if (GetKeyState(VK_CONTROL) < 0)
			pLv->SetItemState(-1, LVIS_SELECTED, LVIS_SELECTED);
		break;
	case 'C':
		if (GetKeyState(VK_CONTROL) < 0)
		{
			CWaitCursor wait;
			CSharedFile fText(GMEM_DDESHARE | GMEM_MOVEABLE | GMEM_ZEROINIT);
			CSharedFile fHtml(GMEM_DDESHARE | GMEM_MOVEABLE | GMEM_ZEROINIT);
			UINT htcf = CDBRow::WriteReport(*pLv, &fText, &fHtml, 1);
			COleDataSource *pDataSource = new COleDataSource;
			pDataSource->CacheGlobalData(CF_TEXT, fText.Detach());
			if (htcf)
				pDataSource->CacheGlobalData(htcf, fHtml.Detach());
			pDataSource->SetClipboard();
			COleDataSource::FlushClipboard();
		}
		break;
	case VK_F2:
		if (HWND hwndEdit = ::FindWindowEx(pNMHDR->hwndFrom, 0, CellEditor_EditClassName, 0))
		{
			LVITEM lvi;
			lvi.iItem = pLv->GetNextItem(-1, LVNI_FOCUSED);
			lvi.iSubItem = 0;
			lvi.mask = LVIF_PARAM;
			if (pLv->GetItem(&lvi) && lvi.lParam != 0)
			{
				lvi.iSubItem = ::GetDlgCtrlID(hwndEdit);
				TextBox(pLv, lvi.iItem, lvi.iSubItem);
			}
		}
		break;
	}
}

void CChildFrame::OnDBRowSetfocusRangeLv(UINT, NMHDR *pNMHDR, LRESULT *pResult)
{
	CellEditor_SubclassListView(pNMHDR->hwndFrom);
	::SendMessage(pNMHDR->hwndFrom, WM_VSCROLL, SB_ENDSCROLL, 0);
}

static SWORD NTAPI Enquote(CString &s, SWORD type)
{
	if (int n = s.GetLength())
		if (s.GetAt(n - 1) == _T('\0'))
			s = CString((LPCWSTR)&*s, n / 2);
	BOOL null = s.IsEmpty();
	switch (type)
	{
	case SQL_CHAR:
	case SQL_VARCHAR:
	case SQL_WCHAR:
	case SQL_WVARCHAR:
	case SQL_BINARY:
	case SQL_VARBINARY:
		CDBRow::Enquote(s);
		break;
	case SQL_DATE:
	case SQL_TIME:
	case SQL_TIMESTAMP:
		CDBRow::Enquote(s);
		s.Insert(0, _T("{ts "));
		s += _T("}");
		break;
	case SQL_GUID:
		CDBRow::Enquote(s);
		s.Insert(0, _T("{guid "));
		s += _T("}");
		break;
	case SQL_LONGVARCHAR:
	case SQL_WLONGVARCHAR:
	case SQL_LONGVARBINARY:
		type = 0;
		break;
	}
	if (null)
		s = _T("NULL");
	return type;
}

static void NTAPI AppendFilter(CString &strFilter, LPCTSTR name, LPCTSTR value)
{
	CString s;
	int ahead = CRecordsetEx::LookAheadSQL(value, _T(','));
	LPCTSTR f = value[abs(ahead)] ? _T(" AND [%s] IN (%s)") :
		StrCmpI(value, _T("NULL")) ? _T(" AND [%s] = %s") :
		_T(" AND [%s] IS %s");
	s.Format(f + (strFilter.IsEmpty() ? 5 : 0), name, value);
	strFilter += s;
}

static void NTAPI AppendUpdate(CString &strUpdate, LPCTSTR name, LPCTSTR value)
{
	CString s;
	s.Format(_T(", [%s] = %s") + (strUpdate.IsEmpty() ? 2 : 0), name, value);
	strUpdate += s;
}

static void NTAPI AppendInsert(CString &strInsert, LPCTSTR name)
{
	CString s;
	s.Format(_T(", [%s]") + (strInsert.IsEmpty() ? 2 : 0), name);
	strInsert += s;
}

static void NTAPI AppendValues(CString &strValues, LPCTSTR value)
{
	CString s;
	s.Format(_T(", %s") + (strValues.IsEmpty() ? 2 : 0), value);
	strValues += s;
}

int CChildFrame::UpdateRow(CListCtrl *pLv, int iRow)
{
	int response = 0;
	CDatabaseEx *pdb = GetDatabase();
	CString sTableName;
	pLv->GetWindowText(sTableName);
	sTableName.GetBufferSetLength(sTableName.ReverseFind(pdb->m_chIDQuoteChar) + 1);
	int cCols = Header_GetItemCount(ListView_GetHeader(pLv->m_hWnd));
	CODBCFieldInfo *rgODBCFieldInfos = (CODBCFieldInfo *)::GetWindowLong(pLv->m_hWnd, GWL_USERDATA);
	CDBRow::Handle *pRow = reinterpret_cast<CDBRow::Handle *>(pLv->GetItemData(iRow));
	CRecordsetEx *prs = CRecordsetEx::New(pdb);
	CString strSQL, strCleanupSQL;
	CString strUpdate, strInsert, strValues, strFilter, strFilterRequery;
	CString strValueUI, strValueDB;
	CString sMsg;
	CComBSTR bstrValueUI;
	try
	{
		SWORD nNullability = 0;
		int iCol = 0;
		while (iCol < cCols)
		{
			CODBCFieldInfo &fieldInfo = rgODBCFieldInfos[iCol];
			nNullability |= fieldInfo.m_nNullability;
			if (!DisallowInPlaceEdit(fieldInfo))
			{
				LPCTSTR name = fieldInfo.m_strName;
				SWORD type = fieldInfo.m_nSQLType;
				SWORD pk = fieldInfo.m_nNullability & CRecordsetEx::nNullabilityPK;
				bstrValueUI.Empty();
				bstrValueUI.m_str = CellEditor_GetCellText(pLv->m_hWnd, iRow, iCol);
				strValueUI = CDBRow::MakeString(bstrValueUI);
				strValueDB = strValueUI;
				if (Enquote(strValueUI, type))
				{
					if (pRow)
					{
						strValueDB = pRow->Item(iCol)->asString();
						CRecordsetEx::FixScale(fieldInfo, strValueDB);
					}
					else if (strValueDB.GetLength())
					{
						AppendInsert(strInsert, name);
						AppendValues(strValues, strValueUI);
					}
					Enquote(strValueDB, type);
					if (pk)
					{
						if (bstrValueUI.Length() == 0)
						{
							sMsg.Format(_T("Sorry but primary key field %s is empty..."), name);
							AfxThrowOleDispatchException(0, sMsg);
						}
						AppendFilter(strFilter, name, strValueDB);
						AppendFilter(strFilterRequery, name, strValueUI);
					}
					if (strValueUI != strValueDB)
					{
						AppendUpdate(strUpdate, name, strValueUI);
					}
				}
				else if (pk)
				{
					sMsg.Format(_T("Sorry but %s primary key involves an unsupported type..."), sTableName);
					AfxThrowOleDispatchException(0, sMsg);
				}
			}
			++iCol;
		}
		if (strInsert.GetLength() || strUpdate.GetLength())
		{
			if (strFilter.IsEmpty())
			{
				sMsg.Format(_T("Sorry but table %s lacks a primary key..."), sTableName);
				AfxThrowOleDispatchException(0, sMsg);
			}
			if (strInsert.GetLength())
			{
				if ((nNullability & CRecordsetEx::nNullabilityAutoIncrement) &&
					pdb->PoweredBy("Microsoft SQL Server"))
				{
					strSQL.Format(_T("SET IDENTITY_INSERT %s ON"), sTableName);
					pdb->Execute(strSQL);
					strCleanupSQL.Format(_T("SET IDENTITY_INSERT %s OFF"), sTableName);
				}
				prs->m_strSQL.Format(_T("INSERT INTO %s (%s) VALUES (%s)"),
					sTableName, strInsert, strValues);
			}
			else
			{
				prs->m_strSQL.Format(_T("UPDATE %s SET %s WHERE %s"),
					sTableName, strUpdate, strFilter);
			}
			prs->m_lOpen = SQL_API_SQLEXECDIRECT;
			prs->Execute();
			prs->Close();
			prs->m_strSQL.Format(_T("SELECT * FROM %s WHERE %s"),
				sTableName, strFilterRequery);
			prs->Execute();
			if (!prs->IsEOF())
			{
				CDBRow::Handle *pUpdatedRow = CDBRow::Scan(*prs);
				pLv->SetItemData(iRow, reinterpret_cast<DWORD>(pUpdatedRow));
				delete pRow;
			}
			prs->Close();
			while (iCol)
			{
				LVITEMW lvi;
				lvi.iSubItem = --iCol;
				lvi.pszText = LPSTR_TEXTCALLBACKW;
				pLv->SendMessage(LVM_SETITEMTEXTW, iRow, (LPARAM)&lvi);
			}
		}
	}
	catch (CException *e)
	{
		response = AppendLog(e, MB_ICONSTOP);
	}
	prs->Release();
	if (strCleanupSQL.GetLength())
	{
		pdb->Execute(strCleanupSQL);
	}
	return response;
}

void CChildFrame::OnDBRowItemchangingRangeLv(UINT, NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLISTVIEW *pNM = (NMLISTVIEW *)pNMHDR;
	if ((pNM->uChanged & LVIF_STATE) &&
		(pNM->uNewState & LVIS_FOCUSED) < (pNM->uOldState & LVIS_FOCUSED))
	{
		if (HWND hwndEdit = CellEditor_ShowEdit(pNMHDR->hwndFrom, FALSE))
		{
			CListCtrl *pLv = static_cast<CListCtrl *>(CWnd::FromHandlePermanent(pNMHDR->hwndFrom));
			if (::GetFocus() != pNMHDR->hwndFrom)
			{
				*pResult = 1;
			}
			else if (::SendMessage(hwndEdit, EM_GETMODIFY, 0, 0))
			{
				*pResult = 1;
				::PostMessage(pNMHDR->hwndFrom, WM_VSCROLL, SB_ENDSCROLL, 0);
			}
			else if (pNM->iItem != pLv->GetItemCount() - 1)
			{
				switch (UpdateRow(pLv, pNM->iItem))
				{
				case IDCANCEL:
					::DestroyWindow(hwndEdit);
					// fall through
				case IDOK:
					*pResult = 1;
				}
			}
		}
	}
}

void CChildFrame::OnDBRowItemchangedRangeLv(UINT, NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLISTVIEW *pNM = (NMLISTVIEW *)pNMHDR;
	if ((pNM->uChanged & LVIF_STATE) &&
		(pNM->uNewState & LVIS_FOCUSED) > (pNM->uOldState & LVIS_FOCUSED) &&
		::GetFocus() == pNMHDR->hwndFrom)
	{
		::PostMessage(pNMHDR->hwndFrom, WM_VSCROLL, SB_ENDSCROLL, 0);
	}
}

void CChildFrame::OnDBRowActivateRangeLv(UINT, NMHDR *pNMHDR, LRESULT *pResult)
{
	NMITEMACTIVATE *pNM = (NMITEMACTIVATE *)pNMHDR;
	if (pNM->iItem != -1 &&
		pNM->lParam != 0 &&
		pNM->iSubItem != -1)
	{
		CListCtrl *pLv = static_cast<CListCtrl *>(CWnd::FromHandlePermanent(pNMHDR->hwndFrom));
		TextBox(pLv, pNM->iItem, pNM->iSubItem);
	}
}

void CChildFrame::OnDBRowClickRangeLv(UINT, NMHDR *pNMHDR, LRESULT *pResult)
{
	NMITEMACTIVATE *pNM = (NMITEMACTIVATE *)pNMHDR;
	CellEditor_SelectCell(pNMHDR->hwndFrom, pNM->iItem, pNM->iSubItem);
}

void CChildFrame::OnDBRowEditSetfocus(UINT nID)
{
	HWND hwndEdit = (HWND)GetCurrentMessage()->lParam;
	HWND hLv = ::GetParent(hwndEdit);
	CListCtrl *pLv = DYNAMIC_DOWNCAST(CListCtrl, CWnd::FromHandlePermanent(hLv));
	ASSERT(pLv);
	CODBCFieldInfo *rgODBCFieldInfos = (CODBCFieldInfo *)::GetWindowLong(pLv->m_hWnd, GWL_USERDATA);
	int iRow = pLv->GetNextItem(-1, LVNI_FOCUSED);
	int iCol = ::GetDlgCtrlID(hwndEdit);
	CODBCFieldInfo &fieldInfo = rgODBCFieldInfos[iCol];
	LONG style = ::GetWindowLong(hwndEdit, GWL_STYLE);
	style &= ~(ES_NUMBER | WS_DISABLED);
	switch (fieldInfo.m_nSQLType)
	{
	case SQL_NUMERIC:
	case SQL_DECIMAL:
	case SQL_INTEGER:
	case SQL_SMALLINT:
	case SQL_TINYINT:
	case SQL_FLOAT:
	case SQL_REAL:
	case SQL_DOUBLE:
		if (fieldInfo.m_nScale == 0)
			style |= ES_NUMBER;
		break;
	}
	BOOL disallowInPlaceEdit = DisallowInPlaceEdit(fieldInfo);
	if (disallowInPlaceEdit)
	{
		::SendMessage(hwndEdit, WM_KILLFOCUS, 0, 0);
		style |= WS_DISABLED;
	}
	::SetWindowLong(hwndEdit, GWL_STYLE, style);
	//::EnableWindow(hwndEdit, !disallowInPlaceEdit);
	::SendMessage(hwndEdit, EM_HIDESELECTION, disallowInPlaceEdit, 0);
}

void CChildFrame::OnDBRowEditKillfocus(UINT nID)
{
	HWND hwndEdit = (HWND)GetCurrentMessage()->lParam;
	HWND hLv = ::GetParent(hwndEdit);
	CListCtrl *pLv = DYNAMIC_DOWNCAST(CListCtrl, CWnd::FromHandlePermanent(hLv));
	ASSERT(pLv);
	if (::SendMessage(hwndEdit, EM_GETMODIFY, 0, 0) == 0)
		return;
	CODBCFieldInfo *rgODBCFieldInfos = (CODBCFieldInfo *)::GetWindowLong(pLv->m_hWnd, GWL_USERDATA);
	int iRow = pLv->GetNextItem(-1, LVNI_FOCUSED);
	int iCol = ::GetDlgCtrlID(hwndEdit);
	CODBCFieldInfo &fieldInfo = rgODBCFieldInfos[iCol];
	TCHAR sz[100];
	switch (fieldInfo.m_nSQLType)
	{
	case SQL_DATE:
	case SQL_TIME:
	case SQL_TIMESTAMP:
		if (::GetWindowText(hwndEdit, sz, 100))
		{
			LPCTSTR fraction = _T("000");
			if (LPTSTR p = StrRChr(sz, NULL, _T('.')))
			{
				if (StrRChr(sz, p, _T('.')) == NULL)
				{
					*p++ = _T('\0');
					fraction = p;
				}
			}
			COleDateTime date;
			if (date.ParseDateTime(sz))
			{
				CString s = date.Format(_T("%Y-%m-%d %H:%M:%S."));
				::SetWindowText(hwndEdit, s + fraction);
				::SendMessage(hwndEdit, EM_SETMODIFY, 1, 0);
			}
			else
			{
				::SetWindowText(hwndEdit, _T("?"));
			}
			::SendMessage(hwndEdit, EM_SETSEL, 0, ::GetWindowTextLength(hwndEdit));
		}
		break;
	}
}

void CChildFrame::OnSortcolumnRangeLv(UINT, NMHDR *pNMHDR, LRESULT *pResult)
{
	CWaitCursor wait;
	CSortListCtrl::From(pNMHDR)->Sort();
}

void CChildFrame::PopulateWb()
{
	CComBSTR bstrPath, bstrURL;
	m_pBrowserApp->get_FullName(&bstrPath);
	LPWSTR lpExtension = PathFindExtensionW(bstrPath);
	SysReAllocStringLen(&bstrPath, 0, lpExtension - bstrPath);
	VarBstrCat(bstrPath, CComBSTR(L".html"), &bstrURL);
	VARIANT varEmpty;
	::VariantInit(&varEmpty);
	m_pBrowserApp->Navigate(bstrURL, &varEmpty, &varEmpty, &varEmpty, &varEmpty);
}

void CChildFrame::PopulatePage(CWnd *pwndPage)
{
	if (!::EnableWindow(pwndPage->m_hWnd, TRUE))
		return;
	switch (pwndPage->GetDlgCtrlID())
	{
	case 1031:
		PopulateLvTables();
		break;
	case 1032:
		PopulateLvProcedures();
		break;
	case 105:
		PopulateWb();
		break;
	}
}

void CChildFrame::OnSelchangeRangeTc(UINT, NMHDR *pNMHDR, LRESULT *pResult)
{
	if (CWnd *pwndPage = ::ShowCurrentPage(pNMHDR->hwndFrom, pNMHDR->code - TCN_SELCHANGING))
	{
		PopulatePage(pwndPage);
		if (::GetFocus() != pNMHDR->hwndFrom)
			pwndPage->SetFocus();
	}
}

void CChildFrame::DeleteDataPages(int iImageMax)
{
	int nPage = m_TcResults.GetItemCount();
	TCITEM tci;
	tci.mask = TCIF_PARAM | TCIF_IMAGE;
	while (nPage > 1)
	{
		m_TcResults.GetItem(--nPage, &tci);
		CWnd *pwndPage = reinterpret_cast<CWnd *>(tci.lParam);
		if (tci.iImage <= iImageMax)
		{
			m_TcResults.DeleteItem(nPage);
			pwndPage->DestroyWindow();
			delete pwndPage;
		}
		else
		{
			pwndPage->ShowWindow(SW_HIDE);
		}
	}
}

BOOL CChildFrame::ClearResults()
{
	GetDocument()->CancelQuery();
	DeleteDataPages();
	m_EdLog.SetReadOnly(FALSE);
	m_EdLog.ClearAll();
	m_EdLog.StyleClearAll();
	m_EdLog.SetStyleBits(5);
	m_EdLog.SetReadOnly(TRUE);
	m_iLogStyle = 0;
	m_TcResults.SetCurSel(0);
	::ShowCurrentPage(m_TcResults.m_hWnd);
	return TRUE;
}

void CChildFrame::AppendSql(LPCTSTR pszSQL)
{
	CRecordsetEx *prs = CRecordsetEx::New(GetDatabase());
	prs->m_strSQL = pszSQL;
	prs->m_lOpen = SQL_API_SQLEXECDIRECT;
}

void CChildFrame::SetLogStyle(long style, LPCTSTR pszStyle)
{
	CStyleDefinition sd;
	sd.Parse(pszStyle);
	sd.SetOneStyle(m_EdLog, (int)style);
}

BSTR CChildFrame::GetLogStyle(long style)
{
	m_iLogStyle = (int)style;
	return 0;
}

void CChildFrame::AppendLogStyled(LPCTSTR pszText)
{
	if (int cchText = lstrlen(pszText))
	{
		m_EdLog.SetReadOnly(FALSE);
		long pos = m_EdLog.GetTextLength();
		m_EdLog.AppendText(cchText, pszText);
		if (pszText[cchText - 1] != '\n')
			m_EdLog.AppendText(1, _T("\n"));
		m_EdLog.StartStyling(pos, '\xFF');
		m_EdLog.SetStyling(cchText, m_iLogStyle);
		m_EdLog.SetReadOnly(TRUE);
	}
}

void CChildFrame::AppendLog(LPCTSTR pszText)
{
	if (int cchText = lstrlen(pszText))
	{
		m_EdLog.SetReadOnly(FALSE);
		m_EdLog.AppendText(cchText, pszText);
		if (pszText[cchText - 1] != '\n')
			m_EdLog.AppendText(1, _T("\n"));
		m_EdLog.SetReadOnly(TRUE);
	}
}

void CChildFrame::OnDestroy() 
{
	DeleteDataPages(127);
	if (m_pBrowserApp)
		m_pBrowserApp->Release();
	if (m_pWebBrowserSite)
		m_pWebBrowserSite->ExternalAddRef();
	CWebBrowserHost::OnDestroy();
	CMDIChildWnd::OnDestroy();
	if (m_pWebBrowserSite)
		m_pWebBrowserSite->ExternalRelease();
}

void CChildFrame::PostNcDestroy() 
{
	if (m_pWebBrowserSite)
		m_pWebBrowserSite->ExternalRelease();
}

void CChildFrame::OnFinalRelease()
{
	if (!DestroyWindow())
		PostNcDestroy();
}

void CChildFrame::OnParentNotify(UINT message, LPARAM lParam) 
{
	switch LOWORD(message)
	{
	case WM_CREATE:
		if (CWnd *pWnd = CWnd::FromHandlePermanent((HWND)lParam))
		{
			if (pWnd->GetStyle() & WS_BORDER)
			{
				pWnd->ModifyStyle(WS_BORDER, 0);
				pWnd->ModifyStyleEx(0, WS_EX_CLIENTEDGE, SWP_FRAMECHANGED);
			}
			pWnd->SendMessage(WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT));
		}
		break;
	case WM_DESTROY:
		if (HIWORD(message) >= 2000 && HIWORD(message) <= 2019)
			if (CODBCFieldInfo *rg = (CODBCFieldInfo *)::SetWindowLong((HWND)lParam, GWL_USERDATA, 0))
				delete[] rg;
		if (m_hWndFocus == (HWND)lParam)
			m_hWndFocus = 0;
		break;
	}
	CMDIChildWnd::OnParentNotify(message, lParam);
}

BOOL CChildFrame::TrackPopupMenu(CMapStringToPtr &rg, CPoint point, BOOL bAllowSubset)
{
	BOOL bEffective = FALSE;
	if (rg.GetCount())
	{
		CMenu menu;
		menu.CreatePopupMenu();
		UINT nID = 101;
		CString key;
		UINT_PTR value;
		POSITION pos = rg.GetStartPosition();
		while (pos)
		{
			rg.GetNextAssoc(pos, key, reinterpret_cast<void *&>(value));
			UINT lower = 0;
			UINT upper = menu.GetMenuItemCount();
			while (lower < upper)
			{
				UINT match = (upper + lower) >> 1;
				CString sMatch;
				menu.GetMenuString(match, sMatch, MF_BYPOSITION);
				int cmp = sMatch.Compare(key);
				if (cmp >= 0)
					upper = match;
				if (cmp <= 0)
					lower = match + 1;
			}
			if (lower <= upper)
			{
				UINT nFlags = MF_BYPOSITION;
				if (bAllowSubset)
					nFlags |= MF_HELP;
				if (value & 0x80000000)
					nFlags |= MF_CHECKED;
				menu.InsertMenu(lower, nFlags, nID, key);
			}
			else
			{
				ASSERT(FALSE);
			}
			++nID;
		}
		menu.InsertMenu(0, MF_BYPOSITION | MF_SEPARATOR);
		menu.InsertMenu(0, MF_BYPOSITION, '*', _T("*"));
		if (bAllowSubset)
		{
			menu.AppendMenu(MF_BYPOSITION | MF_SEPARATOR);
			menu.AppendMenu(MF_BYPOSITION, IDOK, _T("Apply (Use right button or spacebar to check/uncheck items)"));
		}
		ASSERT(m_hMenuDefault == 0);
		m_hMenuDefault = menu.m_hMenu;
		int response = menu.TrackPopupMenu(TPM_RETURNCMD | TPM_NONOTIFY, point.x, point.y, this);
		m_hMenuDefault = 0;
		switch (response)
		{
		case 0: // Menu canceled
			break;
		case 1: // Apply
			nID = 101;
			pos = rg.GetStartPosition();
			while (pos)
			{
				rg.GetNextAssoc(pos, key, reinterpret_cast<void *&>(value));
				UINT_PTR &value = reinterpret_cast<UINT_PTR &>(rg[key]);
				if (menu.GetMenuState(nID++, MF_BYCOMMAND) & MF_CHECKED)
					value |= 0x80000000;
				else
					value &= ~0x80000000;
			}
			bEffective = TRUE;
			break;
		case '*': // Select all
			pos = rg.GetStartPosition();
			while (pos)
			{
				rg.GetNextAssoc(pos, key, reinterpret_cast<void *&>(value));
				reinterpret_cast<UINT_PTR &>(rg[key]) |= 0x80000000;
			}
			bEffective = TRUE;
			break;
		default: // Select one
			pos = rg.GetStartPosition();
			while (pos)
			{
				rg.GetNextAssoc(pos, key, reinterpret_cast<void *&>(value));
				reinterpret_cast<UINT_PTR &>(rg[key]) &= ~0x80000000;
			}
			menu.GetMenuString(response, key, MF_BYCOMMAND);
			reinterpret_cast<UINT_PTR &>(rg[key]) |= 0x80000000;
			bEffective = TRUE;
			break;
		}
	}
	return bEffective;
}

void CChildFrame::RetrieveText(CListCtrl &Lv, BOOL bOnePage)
{
	if (!ClearResults())
		return;
	int i = -1;
	while ((i = Lv.GetNextItem(i, LVNI_SELECTED)) != -1)
	{
		CRecordsetEx *prs = CRecordsetEx::New(GetDatabase());
		CString sMajor = Lv.GetItemText(i, 1);
		CString sMinor = Lv.GetItemText(i, 0);
		prs->m_strTableName.Format(
			bOnePage ? _T("Textual Definition") :
			_T("[%s].[%s] - Textual Definition"), sMajor, sMinor);
		prs->m_strSQL.Format(_T("SP_HELPTEXT '%s.%s'"), sMajor, sMinor);
	}
}

void CChildFrame::RetrieveColumns(BOOL bOnePage)
{
	if (!ClearResults())
		return;
	int i = -1;
	while ((i = m_LvTables.GetNextItem(i, LVNI_SELECTED)) != -1)
	{
		CRecordsetEx *prs = CRecordsetEx::New(GetDatabase());
		CString sMajor = m_LvTables.GetItemText(i, 1);
		CString sMinor = m_LvTables.GetItemText(i, 0);
		prs->m_strTableName.Format(
			bOnePage ? _T("Columns") :
			_T("[%s].[%s] - Columns"), sMajor, sMinor);
		prs->m_strSQL.Format(_T(";[%s];[%s];;"), sMajor, sMinor);
		prs->m_lOpen = SQL_API_SQLCOLUMNS;
	}
}

void CChildFrame::RetrieveSpecialColumns(int type, int scope, int nullable, BOOL bOnePage)
{
	if (!ClearResults())
		return;
	int i = -1;
	while ((i = m_LvTables.GetNextItem(i, LVNI_SELECTED)) != -1)
	{
		CRecordsetEx *prs = CRecordsetEx::New(GetDatabase());
		CString sMajor = m_LvTables.GetItemText(i, 1);
		CString sMinor = m_LvTables.GetItemText(i, 0);
		prs->m_strTableName.Format(
			bOnePage ? _T("Special Columns") :
			_T("[%s].[%s] - Special Columns"), sMajor, sMinor);
		prs->m_strSQL.Format(_T(";[%s];[%s];%d;%d;%d;"), sMajor, sMinor, type, scope, nullable);
		prs->m_lOpen = SQL_API_SQLSPECIALCOLUMNS;
	}
}

void CChildFrame::RetrievePrimaryKeys(BOOL bOnePage)
{
	if (!ClearResults())
		return;
	int i = -1;
	while ((i = m_LvTables.GetNextItem(i, LVNI_SELECTED)) != -1)
	{
		CRecordsetEx *prs = CRecordsetEx::New(GetDatabase());
		CString sMajor = m_LvTables.GetItemText(i, 1);
		CString sMinor = m_LvTables.GetItemText(i, 0);
		prs->m_strTableName.Format(
			bOnePage ? _T("Primary Keys") :
			_T("[%s].[%s] - Primary Keys"), sMajor, sMinor);
		prs->m_strSQL.Format(_T(";[%s];[%s];"), sMajor, sMinor);
		prs->m_lOpen = SQL_API_SQLPRIMARYKEYS;
	}
}

void CChildFrame::RetrieveForeignKeys(BOOL bOnePage)
{
	if (!ClearResults())
		return;
	int i = -1;
	while ((i = m_LvTables.GetNextItem(i, LVNI_SELECTED)) != -1)
	{
		CRecordsetEx *prs = CRecordsetEx::New(GetDatabase());
		CString sMajor = m_LvTables.GetItemText(i, 1);
		CString sMinor = m_LvTables.GetItemText(i, 0);
		prs->m_strTableName.Format(
			bOnePage ? _T("Foreign Keys") :
			_T("[%s].[%s] - Foreign Keys"), sMajor, sMinor);
		prs->m_strSQL.Format(_T(";;;;[%s];[%s];"), sMajor, sMinor);
		prs->m_lOpen = SQL_API_SQLFOREIGNKEYS;
	}
}

void CChildFrame::RetrieveTablePrivileges(BOOL bOnePage)
{
	if (!ClearResults())
		return;
	int i = -1;
	while ((i = m_LvTables.GetNextItem(i, LVNI_SELECTED)) != -1)
	{
		CRecordsetEx *prs = CRecordsetEx::New(GetDatabase());
		CString sMajor = m_LvTables.GetItemText(i, 1);
		CString sMinor = m_LvTables.GetItemText(i, 0);
		prs->m_strTableName.Format(
			bOnePage ? _T("Table Privileges") :
			_T("[%s].[%s] - Table Privileges"), sMajor, sMinor);
		prs->m_strSQL.Format(_T(";[%s];[%s];"), sMajor, sMinor);
		prs->m_lOpen = SQL_API_SQLTABLEPRIVILEGES;
	}
}

void CChildFrame::RetrieveStatistics(int fUnique, int fAccuracy, BOOL bOnePage)
{
	if (!ClearResults())
		return;
	int i = -1;
	while ((i = m_LvTables.GetNextItem(i, LVNI_SELECTED)) != -1)
	{
		CRecordsetEx *prs = CRecordsetEx::New(GetDatabase());
		CString sMajor = m_LvTables.GetItemText(i, 1);
		CString sMinor = m_LvTables.GetItemText(i, 0);
		prs->m_strTableName.Format(
			bOnePage ? _T("Statistics") :
			_T("[%s].[%s] - Statistics"), sMajor, sMinor);
		prs->m_strSQL.Format(_T(";[%s];[%s];%d;%d;"), sMajor, sMinor, fUnique, fAccuracy);
		prs->m_lOpen = SQL_API_SQLSTATISTICS;
	}
}

void CChildFrame::RetrieveProcedureColumns(BOOL bOnePage)
{
	if (!ClearResults())
		return;
	int i = -1;
	while ((i = m_LvProcedures.GetNextItem(i, LVNI_SELECTED)) != -1)
	{
		CRecordsetEx *prs = CRecordsetEx::New(GetDatabase());
		CString sMajor = m_LvProcedures.GetItemText(i, 1);
		CString sMinor = m_LvProcedures.GetItemText(i, 0);
		prs->m_strTableName.Format(
			bOnePage ? _T("Procedure Columns") :
			_T("[%s].[%s] - Procedure Columns"), sMajor, sMinor);
		prs->m_strSQL.Format(_T(";[%s];[%s];;"), sMajor, sMinor);
		prs->m_lOpen = SQL_API_SQLPROCEDURECOLUMNS;
	}
}

void CChildFrame::RetrieveDataTypes(int fSqlType)
{
	if (!ClearResults())
		return;
	CRecordsetEx *prs = CRecordsetEx::New(GetDatabase());
	prs->m_strSQL.Format(_T("[%d];"), fSqlType);
	prs->m_lOpen = SQL_API_SQLGETTYPEINFO;
}

CScintillaCtrl *CChildFrame::CreateTextPage(LPCTSTR pszTitle)
{
	RECT rect;
	m_TcResults.GetClientRect(&rect);
	m_TcResults.AdjustRect(FALSE, &rect);
	m_TcResults.MapWindowPoints(this, &rect);
	TCITEM tci;
	tci.mask = TCIF_TEXT|TCIF_PARAM;
	tci.pszText = (LPTSTR)pszTitle;
	tci.cchTextMax = 0;
	int nPage = m_TcResults.GetItemCount();
	CScintillaCtrl *pEd = new CScintillaCtrl;
	pEd->Create(WS_CHILD|WS_BORDER, rect, this, 2020);
	pEd->StyleSetFont(STYLE_DEFAULT, "Courier New");
	pEd->StyleClearAll();
	tci.lParam = reinterpret_cast<LPARAM>(pEd);
	m_TcResults.SetWindowPos(pEd, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
	m_TcResults.InsertItem(nPage, &tci);
	if (nPage == 1)
	{
		m_TcResults.SetCurSel(nPage);
		pEd->ShowWindow(SW_SHOW);
		m_EdLog.ShowWindow(SW_HIDE);
	}
	return pEd;
}

void CChildFrame::RetrieveDBInfo(BOOL bOnePage)
{
	if (!ClearResults())
		return;
	CWaitCursor wait;
	CSharedFile fText(GMEM_DDESHARE | GMEM_MOVEABLE | GMEM_ZEROINIT);
	CSharedFile fHtml(GMEM_DDESHARE | GMEM_MOVEABLE | GMEM_ZEROINIT);

	UINT htcf = CDBRow::WriteHtcf(&fHtml);

	CDBInfo dbinfo(GetDatabase(), &fText, &fHtml);

	long pos = fText.GetLength();
	dbinfo.WriteDriverInfo();
	fText.Seek(pos, CFile::begin);
	void *lower, *upper;
	int cch = fText.GetBufferPtr(CFile::bufferRead, -1, &lower, &upper);
	CScintillaCtrl *pEd = CreateTextPage(_T("Driver Information"));
	pEd->AppendText(cch, (char *)lower);
	pEd->SetReadOnly(TRUE);

	pos = fText.GetLength();
	dbinfo.WriteFunctions();
	fText.Seek(pos, CFile::begin);
	cch = fText.GetBufferPtr(CFile::bufferRead, -1, &lower, &upper);
	if (bOnePage)
		pEd->SetReadOnly(FALSE);
	else
		pEd = CreateTextPage(_T("Functions"));
	pEd->AppendText(cch, (char *)lower);
	pEd->SetReadOnly(TRUE);

	pos = fText.GetLength();
	dbinfo.WriteSupportedSQL();
	fText.Seek(pos, CFile::begin);
	cch = fText.GetBufferPtr(CFile::bufferRead, -1, &lower, &upper);
	if (bOnePage)
		pEd->SetReadOnly(FALSE);
	else
		pEd = CreateTextPage(_T("Supported SQL"));
	pEd->AppendText(cch, (char *)lower);
	pEd->SetReadOnly(TRUE);

	pos = fText.GetLength();
	dbinfo.WriteDataTypes();
	fText.Seek(pos, CFile::begin);
	cch = fText.GetBufferPtr(CFile::bufferRead, -1, &lower, &upper);
	if (bOnePage)
		pEd->SetReadOnly(FALSE);
	else
		pEd = CreateTextPage(_T("Data Types"));
	pEd->AppendText(cch, (char *)lower);
	pEd->SetReadOnly(TRUE);

	pos = fText.GetLength();
	dbinfo.WriteIdentifiers();
	fText.Seek(pos, CFile::begin);
	cch = fText.GetBufferPtr(CFile::bufferRead, -1, &lower, &upper);
	if (bOnePage)
		pEd->SetReadOnly(FALSE);
	else
		pEd = CreateTextPage(_T("Identifiers"));
	pEd->AppendText(cch, (char *)lower);
	pEd->SetReadOnly(TRUE);

	pos = fText.GetLength();
	dbinfo.WriteLimits();
	fText.Seek(pos, CFile::begin);
	cch = fText.GetBufferPtr(CFile::bufferRead, -1, &lower, &upper);
	if (bOnePage)
		pEd->SetReadOnly(FALSE);
	else
		pEd = CreateTextPage(_T("Limits"));
	pEd->AppendText(cch, (char *)lower);
	pEd->SetReadOnly(TRUE);

	pos = fText.GetLength();
	dbinfo.WriteTransactions();
	fText.Seek(pos, CFile::begin);
	cch = fText.GetBufferPtr(CFile::bufferRead, -1, &lower, &upper);
	if (bOnePage)
		pEd->SetReadOnly(FALSE);
	else
		pEd = CreateTextPage(_T("Transactions"));
	pEd->AppendText(cch, (char *)lower);
	pEd->SetReadOnly(TRUE);

	pos = fText.GetLength();
	dbinfo.WriteMiscellaneous();
	fText.Seek(pos, CFile::begin);
	cch = fText.GetBufferPtr(CFile::bufferRead, -1, &lower, &upper);
	if (bOnePage)
		pEd->SetReadOnly(FALSE);
	else
		pEd = CreateTextPage(_T("Miscellaneous"));
	pEd->AppendText(cch, (char *)lower);
	pEd->SetReadOnly(TRUE);

	htcf = CDBRow::WriteHtcf(&fHtml, htcf);

	COleDataSource *pDataSource = new COleDataSource;
	pDataSource->CacheGlobalData(CF_TEXT, fText.Detach());
	if (htcf)
		pDataSource->CacheGlobalData(htcf, fHtml.Detach());
	pDataSource->SetClipboard();
	COleDataSource::FlushClipboard();
}

void CChildFrame::DropSelectedTables()
{
	CString sMsg;
	int n = m_LvTables.GetSelectedCount();
	sMsg.Format(
		_T("You have chosen to drop %d table%s.\nIs that your intention?"),
		n, &_T("\0s")[n != 1]);
	int response = AfxMessageBox(sMsg, MB_YESNOCANCEL);
	if (response == IDCANCEL || !ClearResults())
		return;
	CMapStringToPtr rgProcessedConstraints;
	long lOpen = response != IDYES ? 9999 : SQL_API_SQLEXECDIRECT;
	int i = -1;
	while (response != IDCANCEL &&
		(i = m_LvTables.GetNextItem(i, LVNI_SELECTED)) != -1)
	{
		CRecordsetEx *prs = CRecordsetEx::New(GetDatabase());
		CString sType, sMajor, sMinor;
		CString sFkTableOwner, sFkTableName, sKeySeq, sFkName;
		CString strSQL;
		try
		{
			sType = m_LvTables.GetItemText(i, 2);
			sMajor = m_LvTables.GetItemText(i, 1);
			sMinor = m_LvTables.GetItemText(i, 0);
			LPCTSTR pszFmt = _T(";;;;[%s];[%s];\0;[%s];[%s];;;;\0");
			while (*pszFmt)
			{
				prs->m_strSQL.Format(pszFmt, sMajor, sMinor);
				prs->m_lOpen = SQL_API_SQLFOREIGNKEYS;
				prs->Execute();
				while (!prs->IsEOF())
				{
					prs->GetFieldValue(5, sFkTableOwner);
					prs->GetFieldValue(6, sFkTableName);
					prs->GetFieldValue(8, sKeySeq);
					prs->GetFieldValue(11, sFkName);
					prs->MoveNext();
					if (StrToInt(sKeySeq) != 1)
						continue;
					strSQL.Format(_T("ALTER TABLE [%s].[%s] DROP CONSTRAINT [%s]"),
						sFkTableOwner, sFkTableName, sFkName);
					if (InterlockedExchangePointer(&rgProcessedConstraints[strSQL], PVOID(1)))
						continue;
					CRecordsetEx *prs = CRecordsetEx::New(GetDatabase());
					prs->m_strSQL = strSQL;
					prs->m_lOpen = lOpen;
				}
				prs->Close();
				pszFmt += lstrlen(pszFmt) + 1;
			}
			CRecordsetEx *prs = CRecordsetEx::New(GetDatabase());
			prs->m_strSQL.Format(_T("DROP %s [%s].[%s]"), sType, sMajor, sMinor);
			prs->m_lOpen = lOpen;
		}
		catch (CException *e)
		{
			response = AppendLog(e, MB_OKCANCEL | MB_ICONSTOP);
		}
		prs->Release();
	}
}

void CChildFrame::ChangeOwnerSelectedTables()
{
	CInputBox dlg(IDD_CHANGE_OWNER, this);
	int response = dlg.RunModalLoop(MLF_SHOWONIDLE);
	CString sNewOwner;
	dlg.GetDlgItemText(100, sNewOwner);
	long lOpen = response != IDYES ? 9999 : SQL_API_SQLEXECDIRECT;
	int i = -1;
	while (response != IDCANCEL &&
		(i = m_LvTables.GetNextItem(i, LVNI_SELECTED)) != -1)
	{
		CString sMajor = m_LvTables.GetItemText(i, 1);
		CString sMinor = m_LvTables.GetItemText(i, 0);
		CRecordsetEx *prs = CRecordsetEx::New(GetDatabase());
		prs->m_strSQL.Format(_T("sp_changeobjectowner '\"%s\".\"%s\"', '%s'"), sMajor, sMinor, sNewOwner);
		prs->m_lOpen = lOpen;
	}
}

void CChildFrame::GrantAllSelectedTables(BOOL f)
{
	CInputBox dlg(IDD_GRANT_ALL, this);
	dlg.SetWindowText(f ? _T("Grant All") : _T("Revoke All"));
	int response = dlg.RunModalLoop(MLF_SHOWONIDLE);
	CString sGrantee;
	dlg.GetDlgItemText(100, sGrantee);
	long lOpen = response != IDYES ? 9999 : SQL_API_SQLEXECDIRECT;
	int i = -1;
	while (response != IDCANCEL &&
		(i = m_LvTables.GetNextItem(i, LVNI_SELECTED)) != -1)
	{
		CString sMajor = m_LvTables.GetItemText(i, 1);
		CString sMinor = m_LvTables.GetItemText(i, 0);
		CRecordsetEx *prs = CRecordsetEx::New(GetDatabase());
		prs->m_strSQL.Format(
			f ? _T("GRANT ALL ON [%s].[%s] TO [%s]") : _T("REVOKE ALL ON [%s].[%s] FROM [%s]"),
			sMajor, sMinor, sGrantee);
		prs->m_lOpen = lOpen;
	}
}

static void NTAPI CreateOnePageItems(CMenu *pMenu, UINT uCount)
{
	UINT i = pMenu->GetMenuItemCount();
	while (UINT j = i)
	{
		if (UINT id = pMenu->GetMenuItemID(--i))
		{
			if (id < 100)
			{
				if (uCount != 1)
				{
					CString text;
					pMenu->GetMenuString(i, text, MF_BYPOSITION);
					text += _T(" - one page");
					pMenu->InsertMenu(j, MF_STRING | MF_BYPOSITION, id + 100, text);
				}
			}
			else if (id < 200)
			{
				pMenu->DeleteMenu(i, MF_BYPOSITION);
			}
		}
	}
}

void CChildFrame::OnContextMenuLvTables(CPoint point)
{
	//CHeaderCtrl *pHd = m_LvTables.GetHeaderCtrl();
	CHeaderCtrl *pHd = static_cast<CHeaderCtrl *>(CWnd::FromHandle(
		reinterpret_cast<HWND>(m_LvTables.SendMessage(LVM_GETHEADER))));
	HDHITTESTINFO ht;
	ht.pt = point;
	pHd->ScreenToClient(&ht.pt);
	BOOL bEffective = FALSE;
	UINT uCount = 0;
	switch (pHd->SendMessage(HDM_HITTEST, 0, (LPARAM)&ht))
	{
	case 1:
		bEffective = TrackPopupMenu(m_rgOwnerTables, point);
		break;
	case 2:
		bEffective = TrackPopupMenu(m_rgTypeTables, point, TRUE);
		break;
	case -1:
		uCount = m_LvTables.GetSelectedCount();
		break;
	}
	if (bEffective)
	{
		CWaitCursor wait;
		PopulateLvTables();
	}
	else if (uCount)
	{
		CMenu *pMenu = GetDocTemplate()->m_menuContext.GetSubMenu(0);
		CreateOnePageItems(pMenu, uCount);
		int response = pMenu->TrackPopupMenu(TPM_RETURNCMD | TPM_NONOTIFY, point.x, point.y, this);
		switch (response)
		{
		case 1:
			RetrieveText(m_LvTables);
			break;
		case 2:
			RetrieveColumns();
			break;
		case 3:
			RetrievePrimaryKeys();
			break;
		case 4:
			RetrieveForeignKeys();
			break;
		case 5:
			RetrieveTablePrivileges();
			break;
		case 6:
			RetrieveStatistics(SQL_INDEX_ALL, SQL_ENSURE);
			break;
		case 7:
			RetrieveSpecialColumns(SQL_BEST_ROWID, SQL_SCOPE_CURROW, SQL_NULLABLE);
			break;
		case 8:
			RetrieveSpecialColumns(SQL_ROWVER, SQL_SCOPE_CURROW, SQL_NULLABLE);
			break;
		case 101:
			RetrieveText(m_LvTables, TRUE);
			break;
		case 102:
			RetrieveColumns(TRUE);
			break;
		case 103:
			RetrievePrimaryKeys(TRUE);
			break;
		case 104:
			RetrieveForeignKeys(TRUE);
			break;
		case 105:
			RetrieveTablePrivileges(TRUE);
			break;
		case 106:
			RetrieveStatistics(SQL_INDEX_ALL, SQL_ENSURE, TRUE);
			break;
		case 107:
			RetrieveSpecialColumns(SQL_BEST_ROWID, SQL_SCOPE_CURROW, SQL_NULLABLE, TRUE);
			break;
		case 108:
			RetrieveSpecialColumns(SQL_ROWVER, SQL_SCOPE_CURROW, SQL_NULLABLE, TRUE);
			break;
		case 1001:
			DropSelectedTables();
			break;
		case 1002:
			ChangeOwnerSelectedTables();
			break;
		case 1003:
			GrantAllSelectedTables(TRUE);
			break;
		case 1004:
			GrantAllSelectedTables(FALSE);
			break;
		}
	}
}

void CChildFrame::OnContextMenuLvProcedures(CPoint point)
{
	//CHeaderCtrl *pHd = m_LvProcedures.GetHeaderCtrl();
	CHeaderCtrl *pHd = static_cast<CHeaderCtrl *>(CWnd::FromHandle(
		reinterpret_cast<HWND>(m_LvProcedures.SendMessage(LVM_GETHEADER))));
	HDHITTESTINFO ht;
	ht.pt = point;
	pHd->ScreenToClient(&ht.pt);
	BOOL bEffective = FALSE;
	UINT uCount = 0;
	switch (pHd->SendMessage(HDM_HITTEST, 0, (LPARAM)&ht))
	{
	case 1:
		bEffective = TrackPopupMenu(m_rgOwnerProcedures, point);
		break;
	case -1:
		uCount = m_LvProcedures.GetSelectedCount();
		break;
	}
	if (bEffective)
	{
		CWaitCursor wait;
		PopulateLvProcedures();
	}
	else if (uCount)
	{
		CMenu *pMenu = GetDocTemplate()->m_menuContext.GetSubMenu(1);
		CreateOnePageItems(pMenu, uCount);
		int response = pMenu->TrackPopupMenu(TPM_RETURNCMD | TPM_NONOTIFY, point.x, point.y, this);
		switch (response)
		{
		case 1:
			RetrieveText(m_LvProcedures);
			break;
		case 2:
			RetrieveProcedureColumns();
			break;
		case 101:
			RetrieveText(m_LvProcedures, TRUE);
			break;
		case 102:
			RetrieveProcedureColumns(TRUE);
			break;
		}
	}
}

void CChildFrame::OnContextMenuCbUse(CPoint point)
{
	CMenu *pMenu = GetDocTemplate()->m_menuContext.GetSubMenu(2);
	pMenu->EnableMenuItem(3, m_CbUse.IsWindowEnabled() ? MF_ENABLED : MF_GRAYED);
	int response = pMenu->TrackPopupMenu(TPM_RETURNCMD | TPM_NONOTIFY, point.x, point.y, this);
	switch (response)
	{
	case 1:
		RetrieveDataTypes(SQL_ALL_TYPES);
		break;
	case 2:
		RetrieveDBInfo();
		break;
	case 3:
		PopulateCbUse();
		break;
	case 102:
		RetrieveDBInfo(TRUE);
		break;
	}
}

void CChildFrame::GenerateSQL(CListCtrl *pLv)
{
	CGenSQLDlg(GetDatabase(), pLv).DoModal();
}

void CChildFrame::DeleteRows(CListCtrl *pLv)
{
	CDatabaseEx *pdb = GetDatabase();
	CString sTableName;
	pLv->GetWindowText(sTableName);
	sTableName.GetBufferSetLength(sTableName.ReverseFind(pdb->m_chIDQuoteChar) + 1);
	CString sMsg;
	int n = pLv->GetSelectedCount();
	sMsg.Format(
		_T("You have chosen to delete %d row%s from table %s.\n")
		_T("Is that your intention?"),
		n, &_T("\0s")[n != 1], sTableName);
	int response = AfxMessageBox(sMsg, MB_YESNO);
	if (response != IDYES)
		return;
	int cCols = Header_GetItemCount(ListView_GetHeader(pLv->m_hWnd));
	CODBCFieldInfo *rgODBCFieldInfos = (CODBCFieldInfo *)::GetWindowLong(pLv->m_hWnd, GWL_USERDATA);
	UINT failopts = MB_OKCANCEL | MB_ICONSTOP;
	pLv->SetFocus();
	CString strFilter;
	CString strValueDB;
	CString strSQL;
	try
	{
		int iRow = -1;
		CDBRow::Handle *pRow;
		while (response != IDCANCEL &&
			(iRow = pLv->GetNextItem(iRow, LVNI_SELECTED)) != -1 &&
			(pRow = reinterpret_cast<CDBRow::Handle *>(pLv->GetItemData(iRow))) != 0) try
		{
			int iCol = 0;
			while (iCol < cCols)
			{
				CODBCFieldInfo &fieldInfo = rgODBCFieldInfos[iCol];
				LPCTSTR name = fieldInfo.m_strName;
				SWORD type = fieldInfo.m_nSQLType;
				SWORD pk = fieldInfo.m_nNullability & CRecordsetEx::nNullabilityPK;
				strValueDB = pRow->Item(iCol)->asString();
				if (pk)
				{
					if (!Enquote(strValueDB, type))
					{
						sMsg.Format(_T("Sorry but %s primary key involves an unsupported type..."), sTableName);
						AfxThrowOleDispatchException(0, sMsg);
					}
					AppendFilter(strFilter, name, strValueDB);
				}
				++iCol;
			}
			if (strFilter.IsEmpty())
			{
				sMsg.Format(_T("Table %s lacks a primary key."), sTableName);
				failopts = MB_ICONSTOP;
				AfxThrowOleDispatchException(0, sMsg);
			}
			strSQL.Format(_T("DELETE FROM %s WHERE %s"), sTableName, strFilter);
			pdb->Execute(pdb->Prepare(strSQL));
			pLv->DeleteItem(iRow--);
		}
		catch (CException *e)
		{
			response = AppendLog(e, failopts);
		}
	}
	catch (CException *e)
	{
		AppendLog(e, MB_ICONSTOP);
	}
}

void CChildFrame::UpdateBlob(CListCtrl *pLv, int row, int col, BSTR data)
{
	CDBRow::Handle *pRow = reinterpret_cast<CDBRow::Handle *>(pLv->GetItemData(row));
	CDatabaseEx *pdb = GetDatabase();
	CString sTableName;
	pLv->GetWindowText(sTableName);
	sTableName.GetBufferSetLength(sTableName.ReverseFind(pdb->m_chIDQuoteChar) + 1);
	int cCols = Header_GetItemCount(ListView_GetHeader(pLv->m_hWnd));
	CODBCFieldInfo *rgODBCFieldInfos = (CODBCFieldInfo *)::GetWindowLong(pLv->m_hWnd, GWL_USERDATA);
	CRecordsetEx *prs = CRecordsetEx::New(GetDatabase());
	CString sMsg;
	CString strFilter;
	CString strValueDB;
	CString strSQL;
	try
	{
		int iCol = 0;
		while (iCol < cCols)
		{
			CODBCFieldInfo &fieldInfo = rgODBCFieldInfos[iCol];
			LPCTSTR name = fieldInfo.m_strName;
			SWORD type = fieldInfo.m_nSQLType;
			SWORD pk = fieldInfo.m_nNullability & CRecordsetEx::nNullabilityPK;
			strValueDB = pRow->Item(iCol)->asString();
			if (pk)
			{
				if (!Enquote(strValueDB, type))
				{
					sMsg.Format(_T("Sorry but %s primary key involves an unsupported type..."), sTableName);
					AfxThrowOleDispatchException(0, sMsg);
				}
				AppendFilter(strFilter, name, strValueDB);
			}
			++iCol;
		}
		if (strFilter.IsEmpty())
		{
			sMsg.Format(_T("Table %s lacks a primary key."), sTableName);
			AfxThrowOleDispatchException(0, sMsg);
		}
		strSQL.Format(_T("UPDATE %s SET [%s] = ? WHERE %s"),
			sTableName, rgODBCFieldInfos[col].m_strName, strFilter);
		CDatabaseEx *pdb = GetDatabase();
		HSTMT hstmt = pdb->Prepare(strSQL);
		SQLINTEGER cbValue = ::SysStringByteLen(data);
		::SQLBindParameter(hstmt,
			1, SQL_PARAM_INPUT, SQL_C_BINARY,
			rgODBCFieldInfos[col].m_nSQLType,
			rgODBCFieldInfos[col].m_nPrecision,
			rgODBCFieldInfos[col].m_nScale,
			data, cbValue, &cbValue);
		pdb->Execute(hstmt);
		prs->m_strSQL.Format(_T("SELECT * FROM %s WHERE %s"), sTableName, strFilter);
		prs->Execute();
		if (!prs->IsEOF())
		{
			CDBRow::Handle *pUpdatedRow = CDBRow::Scan(*prs);
			pLv->SetItemData(row, reinterpret_cast<DWORD>(pUpdatedRow));
			delete pRow;
		}
	}
	catch (CException *e)
	{
		AppendLog(e, MB_ICONSTOP);
	}
	prs->Release();
}

void CChildFrame::TextBox(CListCtrl *pLv, int row, int col)
{
	if (CDBRow::Handle *pRow = reinterpret_cast<CDBRow::Handle *>(pLv->GetItemData(row)))
	{
		CTextBox dlg;
		pRow->Item(col)->GetRef(dlg.m_cds);
		if (dlg.DoModal() == IDOK)
			UpdateBlob(pLv, row, col, dlg.m_text);
	}
}

void CChildFrame::HexBox(CListCtrl *pLv, int row, int col)
{
	if (CDBRow::Handle *pRow = reinterpret_cast<CDBRow::Handle *>(pLv->GetItemData(row)))
	{
		CHexBox dlg;
		pRow->Item(col)->GetRef(dlg.m_cds);
		if (dlg.DoModal() == IDOK)
			UpdateBlob(pLv, row, col, dlg.m_text);
	}
}

void CChildFrame::OnContextMenuTcResults(CPoint point)
{
	TCHITTESTINFO hittest;
	hittest.pt = point;
	m_TcResults.ScreenToClient(&hittest.pt);
	TCITEM tci;
	tci.mask = TCIF_PARAM | TCIF_IMAGE;
	int iItem = m_TcResults.HitTest(&hittest);
	if (!m_TcResults.GetItem(iItem, &tci))
		return;
	CWnd *pWnd = reinterpret_cast<CWnd *>(tci.lParam);
	switch (pWnd->GetDlgCtrlID())
	{
	case 2000: // Table contents
		if (CListCtrl *pLv = DYNAMIC_DOWNCAST(CListCtrl, pWnd))
		{
			CMenu *pMenu = GetDocTemplate()->m_menuContext.GetSubMenu(4);
			pMenu->CheckMenuItem(1, tci.iImage == 1 ? MF_CHECKED : MF_UNCHECKED);
			int response = pMenu->TrackPopupMenu(TPM_RETURNCMD | TPM_NONOTIFY, point.x, point.y, this);
			switch (response)
			{
			case 1:
				tci.iImage = tci.iImage != 1 ? 1 : I_IMAGENONE;
				m_TcResults.SetItem(iItem, &tci);
				break;
			}
		}
		break;
	}
}

void CChildFrame::CreateExcelDocument(CListCtrl *pLv)
{
	CDatabaseEx *const pdb = GetDatabase();
	CString sQualifiedName, sTable;
	pLv->GetWindowText(sQualifiedName);
	pdb->ExtractTableName(sTable, sQualifiedName);
	CFileDialog dlg(FALSE, _T("xls"), sTable,
		OFN_OVERWRITEPROMPT,
		_T("XLS Files (*.xls)|*.xls|")
		_T("All Files (*.*)|*.*|")
		_T("|"));
	if (dlg.DoModal() == IDOK)
	{
		CFile f(dlg.GetPathName(), CFile::modeWrite | CFile::modeCreate);
		BIFF_WriteReport(pLv, &f);
	}
}

void CChildFrame::OnContextMenuLvResults(CListCtrl *pLv, CPoint point)
{
	LVHITTESTINFO hittest;
	hittest.pt = point;
	pLv->ScreenToClient(&hittest.pt);
	//pLv->SubItemHitTest(&hittest);
	pLv->SendMessage(LVM_SUBITEMHITTEST, 0, reinterpret_cast<LPARAM>(&hittest));
	hittest.iItem = pLv->GetNextItem(-1, LVNI_FOCUSED);
	CMenu *pMenu = GetDocTemplate()->m_menuContext.GetSubMenu(3);
	UINT enable = hittest.iItem != -1 &&
		pLv->GetItemData(hittest.iItem) != 0 &&
		hittest.iSubItem != -1 ? MF_ENABLED : MF_GRAYED;
	pMenu->EnableMenuItem(3, enable);
	int response = pMenu->TrackPopupMenu(TPM_RETURNCMD | TPM_NONOTIFY, point.x, point.y, this);
	switch (response)
	{
	case 1:
		GenerateSQL(pLv);
		break;
	case 2:
		DeleteRows(pLv);
		break;
	case 3:
		TextBox(pLv, hittest.iItem, hittest.iSubItem);
		break;
	case 4:
		HexBox(pLv, hittest.iItem, hittest.iSubItem);
		break;
	case 5:
		CreateExcelDocument(pLv);
		break;
	}
}

void CChildFrame::OnContextMenu(CWnd *pWnd, CPoint point)
{
	if (pWnd == this)
	{
		POINT ptClient = point;
		ScreenToClient(&ptClient);
		pWnd = ChildWindowFromPoint(ptClient, CWP_SKIPINVISIBLE|CWP_SKIPTRANSPARENT);
	}
	switch (pWnd->GetDlgCtrlID())
	{
	case 104:
		OnContextMenuCbUse(point);
		break;
	case 1031:
		OnContextMenuLvTables(point);
		break;
	case 1032:
		OnContextMenuLvProcedures(point);
		break;
	case 102:
		OnContextMenuTcResults(point);
		break;
	case 2000:
		OnContextMenuLvResults(static_cast<CListCtrl *>(pWnd), point);
		break;
	}
}

void CChildFrame::AppendForeignKeyFilter(CString &strFilter, LPCTSTR lpTablePK, LPCTSTR lpColumnPK, LPCTSTR lpColumnFK)
{
	CDatabaseEx *const pdb = GetDatabase();
	int i = 0;
	TCITEM tci;
	tci.mask = TCIF_PARAM | TCIF_IMAGE;
	while (m_TcResults.GetItem(++i, &tci) && tci.iImage == CODBCQueryDocTemplate::iImageFilter)
	{
		CListCtrl *pLv = reinterpret_cast<CListCtrl *>(tci.lParam);
		CODBCFieldInfo *rgODBCFieldInfos = (CODBCFieldInfo *)::GetWindowLong(pLv->m_hWnd, GWL_USERDATA);
		CString sMinor, s;
		pLv->GetWindowText(s);
		pdb->ExtractTableName(sMinor, s);
		int j = -1;
		if (sMinor == lpTablePK)
		{
			j += Header_GetItemCount(ListView_GetHeader(pLv->m_hWnd));
			while (j >= 0 && rgODBCFieldInfos[j].m_strName != lpColumnPK)
				--j;
		}
		if (j >= 0)
		{
			CString strValues;
			int i = -1;
			while ((i = pLv->GetNextItem(i, LVNI_SELECTED)) != -1)
			{
				CDBRow::Handle *pRow = reinterpret_cast<CDBRow::Handle *>(pLv->GetItemData(i));
				CString strValue = pRow->Item(j)->asString(0, CDBRow::Item::fEnquoted);
				// Convert a unicode string to octets
				if (int n = strValue.GetLength())
					if (strValue.GetAt(n - 1) == 0)
						strValue = CString((LPCWSTR)&*strValue, n / 2);
				AppendValues(strValues, strValue);
			}
			if (strValues.GetLength())
			{
				AppendFilter(strFilter, lpColumnFK, strValues);
			}
		}
	}
}

void CChildFrame::OnDblclkLvTables(NMHDR *pNMHDR, LRESULT *pResult)
{
	if (!ClearResults())
		return;
	int i = -1;
	while ((i = m_LvTables.GetNextItem(i, LVNI_SELECTED)) != -1)
	{
		CString sMajor = m_LvTables.GetItemText(i, 1);
		CString sMinor = m_LvTables.GetItemText(i, 0);
		CString sType = m_LvTables.GetItemText(i, 2);
		CString strFilter;
		CRecordsetEx *prs = CRecordsetEx::New(GetDatabase());
		// Compose foreign key filter
		CString sTablePK, sColumnPK, sColumnFK;
		try
		{
			prs->m_strSQL.Format(_T(";;;;[%s];[%s];"), sMajor, sMinor);
			prs->m_lOpen = SQL_API_SQLFOREIGNKEYS;
			prs->Execute();
			while (!prs->IsEOF())
			{
				prs->GetFieldValue(2, sTablePK);
				prs->GetFieldValue(3, sColumnPK);
				prs->GetFieldValue(7, sColumnFK);
				AppendForeignKeyFilter(strFilter, sTablePK, sColumnPK, sColumnFK);
				prs->MoveNext();
			}
		}
		catch (CException *e)
		{
			AppendLog(e);
		}
		prs->Release();
		prs = CRecordsetEx::New(GetDatabase());
		prs->m_strTableName.Format(_T("[%s].[%s] - Table Contents"), sMajor, sMinor);
		prs->m_strTableName.Replace(_T("[PUBLIC]."), _T(""));
		prs->m_strTableName.Replace(_T("[]."), _T(""));
		prs->m_strSQL.Format(_T(";[%s];[%s];"), sMajor, sMinor);
		prs->m_strFilter = strFilter;
		if (sType == _T("VIEW"))
			prs->m_lOpen = CRecordsetEx::lOpenView;
		else
			prs->m_lOpen = CRecordsetEx::lOpenReadPrimaryKey;
	}
}

void CChildFrame::OnKeydownRangeLv(UINT, NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVKEYDOWN *pParam = (NMLVKEYDOWN *)pNMHDR;
	switch (pParam->wVKey)
	{
	case 'A':
		if (GetKeyState(VK_CONTROL) < 0)
			ListView_SetItemState(pNMHDR->hwndFrom, -1, LVIS_SELECTED, LVIS_SELECTED);
		break;
	}
}

void CChildFrame::OnDblclkLvProcedures(NMHDR *pNMHDR, LRESULT *pResult)
{
	RetrieveText(m_LvProcedures);
}

BOOL CChildFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra,
	AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// pump through frame FIRST
	if (CWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;
	// then pump through current view
	if (CCmdTarget *pCmdTarget = GetActiveView())
		if (pCmdTarget->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
			return TRUE;
	// last but not least, pump through app
	if (CCmdTarget *pCmdTarget = AfxGetApp())
		if (pCmdTarget->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
			return TRUE;
	return FALSE;
}

void CChildFrame::OnDropdownTb(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMTOOLBAR *pParam = (NMTOOLBAR *)pNMHDR;
	CMenu menu;
	menu.CreatePopupMenu();
	RECT rc;
	int i = (int)SNDMSG(pNMHDR->hwndFrom, TB_COMMANDTOINDEX, pParam->iItem, 0);
	SNDMSG(pNMHDR->hwndFrom, TB_GETITEMRECT, i, (LPARAM)&rc);
	::MapWindowPoints(pNMHDR->hwndFrom, 0, (LPPOINT)&rc, 2);
	i = 0;
	switch (pParam->iItem)
	{
	case ID_ZOOM_IN:
		if (CScintillaCtrl *pCtrl = DYNAMIC_DOWNCAST(CScintillaCtrl, GetFocus()))
		{
			int j = pCtrl->GetZoom();
			while (i < 20)
			{
				TCHAR szText[12];
				wsprintf(szText, _T("%d"), ++i);
				menu.AppendMenu(i == j ? MF_CHECKED : MF_STRING, i, szText);
			}
			i = (short)menu.TrackPopupMenu(TPM_RIGHTALIGN | TPM_RIGHTBUTTON |
				TPM_NONOTIFY | TPM_RETURNCMD, rc.right, rc.bottom, this);
			if (i)
				pCtrl->SetZoom(i);
		}
		break;
	case ID_ZOOM_OUT:
		if (CScintillaCtrl *pCtrl = DYNAMIC_DOWNCAST(CScintillaCtrl, GetFocus()))
		{
			int j = pCtrl->GetZoom();
			while (i > -10)
			{
				TCHAR szText[12];
				wsprintf(szText, _T("%d"), --i);
				menu.AppendMenu(i == j ? MF_CHECKED : MF_STRING, i, szText);
			}
			i = (short)menu.TrackPopupMenu(TPM_RIGHTALIGN | TPM_RIGHTBUTTON |
				TPM_NONOTIFY | TPM_RETURNCMD, rc.right, rc.bottom, this);
			if (i)
				pCtrl->SetZoom(i);
		}
		break;
	}
}

BOOL CChildFrame::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case ID_ZOOM_IN:
	case ID_ZOOM_OUT:
		if (CScintillaCtrl *pCtrl = DYNAMIC_DOWNCAST(CScintillaCtrl, GetFocus()))
		{
			switch MAKELONG(wParam, pCtrl->GetZoom())
			{
			case MAKELONG(ID_ZOOM_IN, 20):
			case MAKELONG(ID_ZOOM_OUT, -10):
				NMTOOLBAR nm;
				nm.hdr.hwndFrom = (HWND)lParam;
				nm.hdr.idFrom = ::GetDlgCtrlID(nm.hdr.hwndFrom);
				nm.hdr.code = TBN_DROPDOWN;
				nm.iItem = wParam;
				SendMessage(WM_NOTIFY, nm.hdr.idFrom, (LPARAM)&nm);
				return TRUE;
			}
		}
		break;
	}
	return CMDIChildWnd::OnCommand(wParam, lParam);
}

void CChildFrame::OnIdleUpdateCmdUI()
{
	CMDIFrameWnd *pMDIFrame = GetMDIFrame();
	if (pMDIFrame->MDIGetActive() == this &&
		pMDIFrame->GetLastActivePopup() == pMDIFrame)
	{
		SetMessageText(m_strStatus);
		if (m_hWndFocus)
		{
			::SetFocus(m_hWndFocus);
			m_hWndFocus = 0;
		}
	}
	CMDIChildWnd::OnIdleUpdateCmdUI();
}

void CChildFrame::OnQueryNew() 
{
	CString sUse;
	if (m_CbUse.IsWindowEnabled())
		m_CbUse.GetWindowText(sUse);
	GetDocTemplate()->Connect(GetDatabase()->GetConnect(), 0, sUse);
}

void CChildFrame::OnEnterIdle(UINT nWhy, CWnd* pWho) 
{
	CMDIChildWnd::OnEnterIdle(nWhy, pWho);
	if (nWhy == MSGF_MENU && m_hMenuDefault)
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
		UINT uState = ::GetMenuState(m_hMenuDefault, nID, MF_BYCOMMAND);
		if (uState & MF_HELP)
		{
			::CheckMenuItem(m_hMenuDefault, nID, uState & MF_CHECKED ^ MF_CHECKED);
			RECT rect;
			pWho->GetClientRect(&rect);
			rect.right = GetSystemMetrics(SM_CXMENUCHECK);
			pWho->RedrawWindow(&rect);
		}
	}
}

LRESULT CChildFrame::OnMenuChar(UINT nChar, UINT nFlags, CMenu* pMenu) 
{
	if (nChar == VK_SPACE)
		return MAKELONG(m_nIDTracking, MNC_SELECT);
	return CMDIChildWnd::OnMenuChar(nChar, nFlags, pMenu);
}

void CChildFrame::OnFileOpen() 
{
	theApp.m_pDocManager->OnFileOpen();
}

BOOL CChildFrame::OnNcActivate(BOOL bActive) 
{
	if (!bActive)
	{
		HWND hwndFocus = ::GetFocus();
		if (::IsChild(m_hWnd, hwndFocus)) do
		{
			m_hWndFocus = hwndFocus;
			hwndFocus = ::GetParent(hwndFocus);
		} while (hwndFocus != m_hWnd);
	}
	return CMDIChildWnd::OnNcActivate(bActive);
}

void CChildFrame::OnUpdateEditCopy(CCmdUI* pCmdUI) 
{
	CScintillaCtrl *pCtrl = DYNAMIC_DOWNCAST(CScintillaCtrl, GetFocus());
	pCmdUI->Enable(pCtrl && pCtrl->GetSelectionStart() != pCtrl->GetSelectionEnd());
}

void CChildFrame::OnUpdateEditCut(CCmdUI* pCmdUI) 
{
	CScintillaCtrl *pCtrl = DYNAMIC_DOWNCAST(CScintillaCtrl, GetFocus());
	pCmdUI->Enable(pCtrl && !pCtrl->GetReadOnly() &&
		pCtrl->GetSelectionStart() != pCtrl->GetSelectionEnd());
}

void CChildFrame::OnUpdateEditClear(CCmdUI* pCmdUI) 
{
	CScintillaCtrl *pCtrl = DYNAMIC_DOWNCAST(CScintillaCtrl, GetFocus());
	pCmdUI->Enable(pCtrl && !pCtrl->GetReadOnly() &&
		pCtrl->GetSelectionStart() < pCtrl->GetTextLength());
}

void CChildFrame::OnUpdateEditPaste(CCmdUI* pCmdUI) 
{
	CScintillaCtrl *pCtrl = DYNAMIC_DOWNCAST(CScintillaCtrl, GetFocus());
	pCmdUI->Enable(pCtrl && pCtrl->CanPaste());
}

void CChildFrame::OnUpdateEditSelectAll(CCmdUI* pCmdUI) 
{
	CScintillaCtrl *pCtrl = DYNAMIC_DOWNCAST(CScintillaCtrl, GetFocus());
	pCmdUI->Enable(pCtrl && pCtrl->GetTextLength() != 0);
}

void CChildFrame::OnUpdateEditUndo(CCmdUI* pCmdUI) 
{
	CScintillaCtrl *pCtrl = DYNAMIC_DOWNCAST(CScintillaCtrl, GetFocus());
	pCmdUI->Enable(pCtrl && pCtrl->CanUndo());
}

void CChildFrame::OnUpdateEditRedo(CCmdUI* pCmdUI) 
{
	CScintillaCtrl *pCtrl = DYNAMIC_DOWNCAST(CScintillaCtrl, GetFocus());
	pCmdUI->Enable(pCtrl && pCtrl->CanRedo());
}

void CChildFrame::OnEditClear() 
{
	if (CScintillaCtrl *pCtrl = DYNAMIC_DOWNCAST(CScintillaCtrl, GetFocus()))
		pCtrl->Clear();
}

void CChildFrame::OnEditCopy() 
{
	if (CScintillaCtrl *pCtrl = DYNAMIC_DOWNCAST(CScintillaCtrl, GetFocus()))
		pCtrl->Copy();
}

void CChildFrame::OnEditCut() 
{
	if (CScintillaCtrl *pCtrl = DYNAMIC_DOWNCAST(CScintillaCtrl, GetFocus()))
		pCtrl->Cut();
}

void CChildFrame::OnEditPaste() 
{
	if (CScintillaCtrl *pCtrl = DYNAMIC_DOWNCAST(CScintillaCtrl, GetFocus()))
		pCtrl->Paste();
}

void CChildFrame::OnEditSelectAll() 
{
	if (CScintillaCtrl *pCtrl = DYNAMIC_DOWNCAST(CScintillaCtrl, GetFocus()))
		pCtrl->SelectAll();
}

void CChildFrame::OnEditUndo() 
{
	if (CScintillaCtrl *pCtrl = DYNAMIC_DOWNCAST(CScintillaCtrl, GetFocus()))
		pCtrl->Undo();
}

void CChildFrame::OnEditRedo() 
{
	if (CScintillaCtrl *pCtrl = DYNAMIC_DOWNCAST(CScintillaCtrl, GetFocus()))
		pCtrl->Redo();
}

void CChildFrame::OnEditCopyHtml() 
{
	if (CScintillaCtrl *pCtrl = DYNAMIC_DOWNCAST(CScintillaCtrl, GetFocus()))
	{
		CStyleDefinition rgsd[STYLE_MAX + 1];

		CSharedFile fHtml(GMEM_DDESHARE | GMEM_MOVEABLE | GMEM_ZEROINIT);

		UINT htcf = CDBRow::WriteHtcf(&fHtml);
		rgsd->WriteHTML(&fHtml, *pCtrl);
		htcf = CDBRow::WriteHtcf(&fHtml, htcf);
		HGLOBAL hGlobal = fHtml.Detach();
		COleDataSource *pDataSource = new COleDataSource;
		if (htcf)
			pDataSource->CacheGlobalData(htcf, hGlobal);
		pDataSource->CacheGlobalData(CF_TEXT, hGlobal);
		pDataSource->SetClipboard();
		COleDataSource::FlushClipboard();
	}
}

void CChildFrame::OnZoomIn() 
{
	if (CScintillaCtrl *pCtrl = DYNAMIC_DOWNCAST(CScintillaCtrl, GetFocus()))
		pCtrl->ZoomIn();
}

void CChildFrame::OnZoomOut() 
{
	if (CScintillaCtrl *pCtrl = DYNAMIC_DOWNCAST(CScintillaCtrl, GetFocus()))
		pCtrl->ZoomOut();
}

void CChildFrame::OnZoomZero() 
{
	if (CScintillaCtrl *pCtrl = DYNAMIC_DOWNCAST(CScintillaCtrl, GetFocus()))
		pCtrl->SetZoom(0);
}

void CChildFrame::OnUpdateZoom(CCmdUI *pCmdUI)
{
	CScintillaCtrl *pCtrl = DYNAMIC_DOWNCAST(CScintillaCtrl, GetFocus());
	pCmdUI->Enable(pCtrl != 0);
}

void CChildFrame::OnUpdateZoomZero(CCmdUI *pCmdUI)
{
	CScintillaCtrl *pCtrl = DYNAMIC_DOWNCAST(CScintillaCtrl, GetFocus());
	pCmdUI->Enable(pCtrl && pCtrl->GetZoom() != 0);
}

void CChildFrame::OnUpdateOvrIndicator(CCmdUI *pCmdUI)
{
	CScintillaCtrl *pCtrl = DYNAMIC_DOWNCAST(CScintillaCtrl, GetFocus());
	pCmdUI->Enable(pCtrl != 0 && pCtrl->GetOvertype());
}

LPDISPATCH CChildFrame::rdoConnxn()
{
	LPDISPATCH lpDispatch = GetDocument()->m_db.m_rdoConnxn;
	if (lpDispatch)
		lpDispatch->AddRef();
	return lpDispatch;
}

LPDISPATCH CChildFrame::rdoEngine()
{
	LPDISPATCH lpDispatch = CDatabaseEx::m_rdoEngine;
	if (lpDispatch)
		lpDispatch->AddRef();
	return lpDispatch;
}

LPDISPATCH CChildFrame::ShowConsole(long nCmdShow)
{
	static const TCHAR szClass[] = _T("ConsoleWindowClass");
	DWORD dwProcessId = ::GetCurrentProcessId();
	DWORD dwProcessIdConsole = 0;
	HWND hwndConsole = 0;
	void *pv = 0;
	TCHAR path[MAX_PATH];
	CString strPath(path, ::GetModuleFileName(0, path, MAX_PATH));
	strPath.Replace('\\', '_');
	strPath.Insert(0, _T("Console\\"));
	HUSKEY huskey = 0;
	if (ERROR_SUCCESS == ::SHRegCreateUSKey(strPath, KEY_WRITE, 0, &huskey, SHREGSET_FORCE_HKCU))
	{
		static const TCHAR faceName[] = _T("Lucida Console");
		static const DWORD fontFamily = FF_MODERN | 6;
		static const DWORD fontSize = 0xE0000;
		static const DWORD fontWeight = FW_BOLD;
		::SHRegWriteUSValue(huskey, _T("FaceName"), REG_SZ, faceName, sizeof faceName - sizeof(TCHAR), SHREGSET_FORCE_HKCU);
		::SHRegWriteUSValue(huskey, _T("FontFamily"), REG_DWORD, &fontFamily, sizeof fontFamily, SHREGSET_FORCE_HKCU);
		::SHRegWriteUSValue(huskey, _T("FontSize"), REG_DWORD, &fontSize, sizeof fontSize, SHREGSET_FORCE_HKCU);
		::SHRegWriteUSValue(huskey, _T("FontWeight"), REG_DWORD, &fontWeight, sizeof fontWeight, SHREGSET_FORCE_HKCU);
		::SHRegCloseUSKey(huskey);
	}
	if (::AllocConsole())
		::SetConsoleCtrlHandler(ConsoleCtrlHandler, TRUE);
	::SHDeleteKey(HKEY_CURRENT_USER, strPath);
	::SetConsoleTitle(AfxGetAppName());
	::SetConsoleOutputCP(::GetACP());
	while (dwProcessIdConsole != dwProcessId &&
		(hwndConsole = ::FindWindowEx(0, hwndConsole, szClass, 0)) != 0)
	{
		::GetWindowThreadProcessId(hwndConsole, &dwProcessIdConsole);
	}
	if (hwndConsole)
	{
		::SendMessage(hwndConsole, WM_SETREDRAW, 0, 0);
		::ShowWindow(hwndConsole, nCmdShow);
		if (HMENU hMenu = ::GetSystemMenu(hwndConsole, FALSE))
			::DeleteMenu(hMenu, SC_CLOSE, MF_BYCOMMAND);
		::AccessibleObjectFromWindow(hwndConsole, OBJID_WINDOW, IID_IAccessible, &pv);
	}
	return reinterpret_cast<IAccessible *>(pv);
}

BOOL CChildFrame::FreeConsole()
{
	return ::FreeConsole();
}

void CChildFrame::OnStatusTextChange(LPCTSTR pszText)
{
	m_strStatus = pszText;
}
