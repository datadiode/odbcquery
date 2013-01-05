#include "stdafx.h"
#include "resource.h"
#include "helpers.h"
#include "ODBCQuery.h"
#include "CryptSession.h"
#include "WebLinkButton.h"
#include "../Common/VersionData.h"
#include "DBRow.h"

#include "MainFrm.h"
#include "ODBCQueryDoc.h"
#include "ChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const TCHAR g_szConnSection[] = _T("Recent Connection List");
static const TCHAR g_szConnEntry[] = _T("Connection%d");

static void HexDump(LPTSTR pch, BYTE *pb, DWORD cb)
{
	while (cb)
	{
		pch += wsprintf(pch, _T("%02x"), (UINT)*pb++);
		--cb;
	}
}

static void HexScan(LPCTSTR pch, BYTE *pb, DWORD cb)
{
	while (cb)
	{
		TCHAR sz[] = { '0', 'x', *pch++, *pch++, '\0' };
		int iRet = 0;
		StrToIntEx(sz, STIF_SUPPORT_HEX, &iRet);
		*pb++ = (BYTE)iRet;
		--cb;
	}
}

const BYTE CODBCQueryDocTemplate::rgLocalPassword[20] = 
	{'M','M','P','z','I','6','C','@','H','a','A','0','N','i','L','*','I','%','L','l'};

CODBCQueryDocTemplate::CODBCQueryDocTemplate(
	UINT nIDResource, CRuntimeClass *pDocClass,
	CRuntimeClass *pFrameClass, CRuntimeClass *pViewClass,
	UINT nStart, LPCTSTR lpszSection, LPCTSTR lpszEntryFormat, int nSize,
	int nMaxDispLen)
: CMultiDocTemplate(nIDResource, pDocClass, pFrameClass, pViewClass)
, CRecentFileList(nStart, lpszSection, lpszEntryFormat, nSize, nMaxDispLen)
{
	ReadList();
	CCryptSession cryptSession(rgLocalPassword, sizeof rgLocalPassword, TRUE);
	BOOL bWriteList = FALSE;
	for (int iMRU = 0 ; iMRU < m_nSize ; ++iMRU)
	{
		CString &s = m_arrNames[iMRU];
		if (int i = s.Find(_T(";PWD=")) + 1)
		{
			int j = s.Find(';', i += 4);
			if (j == -1)
				j = s.GetLength();
			CString sHex(LPCTSTR(s) + i, j -= i);
			s.Delete(i, j);
			DWORD cb = j / 2;
			CString sPwd;
			LPBYTE pb = (LPBYTE)sPwd.GetBufferSetLength(cb / sizeof(TCHAR));
			HexScan(sHex, pb, cb);
			if (cryptSession.Decrypt(pb, cb))
			{
				s.Insert(i, sPwd);
			}
			else
			{
				s.Delete(i - 5, 5);
				bWriteList = TRUE;
			}
		}
	}
	if (bWriteList)
		WriteList();
	m_menuContext.LoadMenu(IDR_CONTEXT);
	m_imagelist.Create(IDB_IMAGELIST, 16, 0, RGB(255,255,255));
	m_imagelist.SetBkColor(GetSysColor(COLOR_BTNFACE));
}

CODBCQueryDocTemplate::~CODBCQueryDocTemplate()
{
	CCryptSession cryptSession(rgLocalPassword, sizeof rgLocalPassword, TRUE);
	for (int iMRU = 0 ; iMRU < m_nSize ; ++iMRU)
	{
		CString &s = m_arrNames[iMRU];
		if (int i = s.Find(_T(";PWD=")) + 1)
		{
			int j = s.Find(';', i += 4);
			if (j == -1)
				j = s.GetLength();
			CString sPwd(LPCTSTR(s) + i, j -= i);
			s.Delete(i, j);
			DWORD cb = j * sizeof(TCHAR);
			LPBYTE pb = (LPBYTE)sPwd.LockBuffer();
			if (cryptSession.Encrypt(pb, cb))
			{
				CString sHex;
				HexDump(sHex.GetBufferSetLength(2 * cb), pb, cb);
				s.Insert(i, sHex);
			}
			else
			{
				s.Delete(i - 5, 5);
			}
		}
	}
	WriteList();
}

void CODBCQueryDocTemplate::RemoveAttribute(CString &s, LPCTSTR lpAttr)
{
	if (int i = s.Find(lpAttr) + 1)
	{
		int j = s.Find(*lpAttr, i--);
		if (j == -1)
			j = s.GetLength();
		s.Delete(i, j - i);
	}
}

void CODBCQueryDocTemplate::Add(LPCTSTR lpName)
{
	CString sName = lpName;
	RemoveAttribute(sName, _T(";PWD="));
	// update the MRU list, if an existing MRU string matches file name
	int iMRU = 0;
	while (iMRU < m_nSize - 1) // && m_arrNames[iMRU].CompareNoCase(lpszPathName))
	{
		CString sMRU = m_arrNames[iMRU];
		RemoveAttribute(sMRU, _T(";PWD="));
		if (sMRU.CompareNoCase(sName) == 0)
			break;
		++iMRU;
	}
	// move MRU strings before this one down
	while (iMRU)
	{
		m_arrNames[iMRU] = m_arrNames[iMRU - 1];
		--iMRU;
	}
	// place this one at the beginning
	m_arrNames[0] = lpName;
}

void CODBCQueryDocTemplate::UpdateMenu(CCmdUI *pCmdUI)
{
	CMenu *pMenu = pCmdUI->m_pMenu;
	if (m_strOriginal.IsEmpty() && pMenu != NULL)
		pMenu->GetMenuString(pCmdUI->m_nID, m_strOriginal, MF_BYCOMMAND);

	if (m_arrNames[0].IsEmpty())
	{
		// no MRU files
		if (!m_strOriginal.IsEmpty())
			pCmdUI->SetText(m_strOriginal);
		pCmdUI->Enable(FALSE);
		return;
	}

	if (pCmdUI->m_pMenu == NULL)
		return;

	int iMRU;
	for (iMRU = 0; iMRU < m_nSize; iMRU++)
		pCmdUI->m_pMenu->DeleteMenu(pCmdUI->m_nID + iMRU, MF_BYCOMMAND);

	CString strTemp;
	for (iMRU = 0; iMRU < m_nSize; iMRU++)
	{
		CString strTemp = m_arrNames[iMRU];
		if (strTemp.IsEmpty())
			break;
		// double up any '&' characters so they are not underlined
		strTemp.Replace(_T("&"), _T("&&"));
		RemoveAttribute(strTemp, _T(";PWD="));
		// insert mnemonic + the file name
		TCHAR buf[10];
		wsprintf(buf, _T("&%d "), (iMRU + 1 + m_nStart) % 10);
		pCmdUI->m_pMenu->InsertMenu(pCmdUI->m_nIndex++,
			MF_STRING | MF_BYPOSITION, pCmdUI->m_nID++,
			buf + strTemp);
	}
	// update end menu count
	pCmdUI->m_nIndex--; // point to last menu added
	pCmdUI->m_nIndexMax = pCmdUI->m_pMenu->GetMenuItemCount();
	pCmdUI->m_bEnableChanged = TRUE;    // all the added items are enabled
}

CODBCQueryDoc *CODBCQueryDocTemplate::Connect(LPCTSTR szConnect, LPCTSTR szFileName, LPCTSTR szUse, BOOL bMakeVisible)
{
	if (szConnect && CRecordset::FindSQLToken(szConnect, _T("ODBC;")) == szConnect)
	{
		szConnect += 5;
	}
	CODBCQueryDoc *pDocument = static_cast<CODBCQueryDoc *>(
		CMultiDocTemplate::OpenDocumentFile(szFileName, bMakeVisible));
	if (pDocument)
	{
		int response = IDCANCEL;
		do try
		{
			TCHAR szPath[MAX_PATH];
			::GetModuleFileName(0, szPath, MAX_PATH);
			::PathRenameExtension(szPath, _T(".ini"));
			DWORD dwLoginTimeout = GetPrivateProfileInt(_T("ODBC.Settings"), _T("LoginTimeout"), 15, szPath);
			DWORD dwQueryTimeout = GetPrivateProfileInt(_T("ODBC.Settings"), _T("QueryTimeout"), 15, szPath);
			pDocument->m_db.SetLoginTimeout(dwLoginTimeout);
			pDocument->m_db.SetQueryTimeout(dwQueryTimeout);
			if (pDocument->m_db.OpenEx(szConnect))
			{
				CString sUse = szUse;
				if (sUse.GetLength())
				{
					sUse.Insert(0, _T("USE "));
					pDocument->m_db.ExecuteSQL(sUse);
				}
				response = IDOK;
				Add(pDocument->m_db.GetConnect());
				pDocument->UpdateFrameCounts();
				CFrameWnd *pFrame = pDocument->GetView()->GetParentFrame();
				static_cast<CChildFrame *>(pFrame)->OnInitialUpdate();
			}
		}
		catch (CException *e)
		{
			response = 0;
			if (COleDispatchException *p = DYNAMIC_DOWNCAST(COleDispatchException, e))
				if (p->m_scError == 0x800A9C7B)
					response = IDCANCEL;
			if (response == 0)
				response = e->ReportError(MB_RETRYCANCEL | MB_ICONSTOP);
			e->Delete();
		} while (response == IDRETRY);
		if (response == IDCANCEL)
		{
			pDocument->OnCloseDocument();
			pDocument = 0;
		}
	}
	return pDocument;
}

CDocument *CODBCQueryDocTemplate::OpenDocumentFile(LPCTSTR szFileName, BOOL bMakeVisible)
{
	return Connect(0, szFileName, 0);
}

void CODBCQueryDocTemplate::InitialUpdateFrame(CFrameWnd *pFrame, CDocument *pDoc, BOOL bMakeVisible)
{
	if (m_docList.GetCount() == 1 && bMakeVisible)
	{
		CMultiDocTemplate::InitialUpdateFrame(pFrame, pDoc, FALSE);
		pDoc->SendInitialUpdate();
		pFrame->ActivateFrame(SW_MAXIMIZE);
	}
	else
	{
		CMultiDocTemplate::InitialUpdateFrame(pFrame, pDoc, bMakeVisible);
	}
}

CODBCQueryDoc *CODBCQueryDocTemplate::FindDocument(LPCTSTR lpCmdLine) const
{
	POSITION pos = GetFirstDocPosition();
	while (pos)
	{
		CODBCQueryDoc *pDocument = static_cast<CODBCQueryDoc *>(GetNextDoc(pos));
		if (pDocument->m_strCmdLine == lpCmdLine)
			return pDocument;
	}
	return 0;
}

void CODBCQueryDocTemplate::ParseCommandLine(LPTSTR lpCmdLine)
{
	while (*(lpCmdLine += StrSpn(lpCmdLine, TEXT("\t \r\n"))))
	{
		LPTSTR lpCmdNext = PathGetArgs(lpCmdLine);
		PathRemoveArgs(lpCmdLine);
		PathUnquoteSpaces(lpCmdLine);
		CString strPath = lpCmdLine;
		if (PathMatchSpec(strPath, _T("*.DSN")))
		{
			strPath.Insert(0, _T("FILEDSN="));
		}
		if (CODBCQueryDoc *pDocument = FindDocument(strPath))
		{
			pDocument->GetView()->GetParentFrame()->ActivateFrame();
		}
		else if (CODBCQueryDoc *pDocument = Connect(strPath, 0, 0))
		{
			pDocument->m_strCmdLine = strPath;
		}
		lpCmdLine = lpCmdNext;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CODBCQueryApp

BEGIN_MESSAGE_MAP(CODBCQueryApp, CWinApp)
	//{{AFX_MSG_MAP(CODBCQueryApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//}}AFX_MSG_MAP
	ON_UPDATE_COMMAND_UI(0xE010, OnUpdateRecentConnMenu)
	ON_COMMAND_EX_RANGE(0xE010, 0xE01F, OnOpenRecentConn)
	// Dateibasierte Standard-Dokumentbefehle
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	// Standard-Druckbefehl "Seite einrichten"
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

CODBCQueryApp::CODBCQueryApp()
: m_pDocTemplate(0)
{
}

CODBCQueryApp theApp;

BOOL CODBCQueryApp::InitInstance()
{
	LPCTSTR lpClassName = CMainFrame::RegisterWndClass();
	/*if (CODBCFieldInfo *rg = new CODBCFieldInfo[3])
	{
		rg[0].m_strName.FormatV(_T("potential leak %s"), (va_list)&lpClassName);
		delete[] rg;
	}*/
	if (HWND hwndOther = ::FindWindow(lpClassName, 0))
	{
		COPYDATASTRUCT cds;
		cds.dwData = 0;
		cds.cbData = lstrlen(m_lpCmdLine) * sizeof(TCHAR);
		cds.lpData = m_lpCmdLine;
		::SendMessage(hwndOther, WM_COPYDATA, 0, (LPARAM)&cds);
		return FALSE;
	}

	// Initialize OLE (apparently required by COleDataSource)
	if (!AfxOleInit())
	{
		AfxMessageBox(_T("Unable to initialize OLE"), MB_ICONSTOP);
		return FALSE;
	}

	::LoadLibrary(TEXT("RICHED20.DLL"));

	try
	{
		CDatabaseEx::Initialize();
	}
	catch (CException *e)
	{
		// Fail silently if RDO is unavailable
		e->Delete();
	}
	// Initialize the Scintilla source code editing component
	TCHAR path[MAX_PATH];
	GetModuleFileName(NULL, path, _countof(path));
	PathRemoveFileSpec(path);
	PathAppend(path, _T("SciLexer.dll"));
	if (::LoadLibrary(path) == NULL)
	{
		AfxMessageBox(_T("Unable to load SciLexer.dll"), MB_ICONSTOP);
		return FALSE;
	}

	AfxEnableControlContainer();

	SetRegistryKey(_T("Jochen Neubeck"));
	LoadStdProfileSettings(9);

	CMainFrame *pMainFrame = new CMainFrame;

	::SQLSetConnectOption(0, SQL_OPT_TRACE, FALSE);
	if (GetProfileInt(_T("Settings"), _T("ODBCTrace"), 0))
		pMainFrame->OnHelpTrace();

	m_pDocTemplate = new CODBCQueryDocTemplate(
		IDR_ODBCQUTYPE,
		RUNTIME_CLASS(CODBCQueryDoc),
		RUNTIME_CLASS(CChildFrame),
		0,
		0, g_szConnSection, g_szConnEntry, 9);
	AddDocTemplate(m_pDocTemplate);

	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;
	pMainFrame->ShowWindow(SW_MAXIMIZE); //m_nCmdShow
	pMainFrame->RecalcLayout(); // Try fix an occasional glitch on VISTA
	pMainFrame->UpdateWindow();

	m_pDocTemplate->ParseCommandLine(m_lpCmdLine);

	return TRUE;
}

int CODBCQueryApp::ExitInstance()
{
	BOOL bTrace = FALSE;
	::SQLGetConnectOption(0, SQL_OPT_TRACE, &bTrace);
	WriteProfileInt(_T("Settings"), _T("ODBCTrace"), bTrace);
	CDatabaseEx::Uninitialize();
	return CWinApp::ExitInstance();
}

CDocument *CODBCQueryApp::OpenDocumentFile(LPCTSTR lpszFileName) 
{
	CDocument *pDoc = 0;
	CMainFrame *pMainFrame = static_cast<CMainFrame *>(m_pMainWnd);
	if (CMDIChildWnd *pFrame = pMainFrame->MDIGetActive())
	{
		CDocument *pActiveDocument = pFrame->GetActiveDocument();
		ASSERT(pActiveDocument);
		if (pActiveDocument->SaveModified() && pActiveDocument->OnOpenDocument(lpszFileName))
		{
			pActiveDocument->SetPathName(lpszFileName);
			pDoc = pActiveDocument;
		}
	}
	else
	{
		pDoc = m_pDocTemplate->Connect(0, lpszFileName, 0);
	}
	return pDoc;
}

void CODBCQueryApp::OnUpdateRecentConnMenu(CCmdUI* pCmdUI)
{
	m_pDocTemplate->UpdateMenu(pCmdUI);
}

BOOL CODBCQueryApp::OnOpenRecentConn(UINT nID)
{
	ASSERT(nID >= 0xE010);
	ASSERT(nID < 0xE010 + (UINT)m_pDocTemplate->GetSize());
	int nIndex = nID - 0xE010;
	ASSERT((*m_pDocTemplate)[nIndex].GetLength() != 0);
	m_pDocTemplate->Connect((*m_pDocTemplate)[nIndex], 0, 0);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialogdaten
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// Überladungen für virtuelle Funktionen, die vom Anwendungs-Assistenten erzeugt wurden
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	//}}AFX_VIRTUAL

// Implementierung
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//}}AFX_MSG
	void OnWebLinkButton(UINT nIDCtl);
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_WM_DRAWITEM()
	ON_WM_SETCURSOR()
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
	ON_COMMAND_RANGE(1001, 1010, OnWebLinkButton)
	ON_CONTROL_RANGE(BN_SETFOCUS, 1001, 1010, SetDefID)
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if (const CVersionData *pRes = CVersionData::Load())
	{
		TCHAR szVersion[40];
		const VS_FIXEDFILEINFO *pFixedFileInfo = (const VS_FIXEDFILEINFO *)pRes->Data();
		int cch = wsprintf(szVersion, TEXT("ODBCQuery %u.%02u"),
			UINT HIWORD(pFixedFileInfo->dwFileVersionMS),
			UINT LOWORD(pFixedFileInfo->dwFileVersionMS));
		if (pFixedFileInfo->dwFileFlags & VS_FF_PRERELEASE)
			szVersion[cch++] = 'b';
		if (pFixedFileInfo->dwFileFlags & VS_FF_DEBUG)
			szVersion[cch++] = 'D';
		szVersion[cch] = '\0';
		SetDlgItemText(IDC_ST_VERSION, szVersion);
		if (pFixedFileInfo->dwFileDateMS)
		{
			FILETIME ft = { pFixedFileInfo->dwFileDateLS, pFixedFileInfo->dwFileDateMS };
			CoFileTimeNow(&ft);
			SYSTEMTIME st;
			FileTimeToSystemTime(&ft, &st);
			const LCID locale = MAKELCID(MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED), SORT_DEFAULT);
			::GetDateFormat(locale, LOCALE_NOUSEROVERRIDE, &st, 0, szVersion, 40);
			SetDlgItemText(IDC_ST_VERDATE, szVersion);
		}
	}

	::MakeWebLinkButton(m_hWnd, 1001, _T("http://www.gnu.org/licenses/old-licenses/gpl-2.0.html\0"));
	::MakeWebLinkButton(m_hWnd, 1002, _T("http://www.naughter.com/scintilla.html\0"));
	::MakeWebLinkButton(m_hWnd, 1003, _T("http://www.scintilla.org\0"));
	::MakeWebLinkButton(m_hWnd, 1004, _T("http://www.codeproject.com/database/isqlmfc.asp\0"));
	::MakeWebLinkButton(m_hWnd, 1005, _T("http://www.vbaccelerator.com\0"));
	::MakeWebLinkButton(m_hWnd, 1006, _T("ScintillaLicense.txt\0"));
	::MakeWebLinkButton(m_hWnd, 1007, _T("http://activex.microsoft.com/controls/vc/mfc42.cab\0"));
	::MakeWebLinkButton(m_hWnd, 1008, _T("http://www.7-Zip.org\0"));
	::MakeWebLinkButton(m_hWnd, 1009, _T("http://activex.microsoft.com/controls/vb6/msrdo20.cab\0"));
	::MakeWebLinkButton(m_hWnd, 1010, _T("http://www.7-Zip.org\0"));
	return TRUE;
}

void CODBCQueryApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

void CAboutDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	if (nIDCtl >= 1001 && nIDCtl <= 1010)
	{
		DrawWebLinkButton(lpDrawItemStruct);
	}
	CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

BOOL CAboutDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	int nIDCtl = pWnd->GetDlgCtrlID();
	if (nIDCtl >= 1001 && nIDCtl <= 1010)
	{
		if (HMODULE hModule = ::GetModuleHandle(_T("COMCTL32")))
			if (HCURSOR hCursor = ::LoadCursor(hModule, MAKEINTRESOURCE(108)))
				::SetCursor(hCursor);
		LPCTSTR pszURL = HrefWebLinkButton(pWnd->m_hWnd);
		GetTopLevelFrame()->SetMessageText(pszURL);
		return TRUE;
	}
	GetTopLevelFrame()->SetMessageText(AFX_IDS_IDLEMESSAGE);
	return CDialog::OnSetCursor(pWnd, nHitTest, message);
}

void CAboutDlg::OnWebLinkButton(UINT nIDCtl)
{
	if (LPCTSTR pszURL = OpenWebLinkButton())
	{
		HMODULE hModule = ::GetModuleHandle(0);
		if (HRSRC hRes = FindResource(hModule, pszURL, RT_HTML))
		{
			DWORD cbRes = SizeofResource(hModule, hRes);
			PVOID pvRes = LoadResource(hModule, hRes);
			AfxMessageBox(CString((LPCSTR)pvRes, cbRes), MB_ICONINFORMATION);
		}
		else
		{
			::ShellExecute(0, _T("open"), pszURL, 0, 0, SW_SHOWNORMAL);
		}
	}
}

void CAboutDlg::OnContextMenu(CWnd *pWnd, CPoint point) 
{
	int nIDCtl = pWnd->GetDlgCtrlID();
	if (nIDCtl >= 1001 && nIDCtl <= 1010)
	{
		if (LPCTSTR pszURL = HrefWebLinkButton(pWnd->m_hWnd))
		{
			CMenu menu;
			menu.CreatePopupMenu();
			menu.AppendMenu(MF_STRING, 1, _T("Copy link"));
			int response = menu.TrackPopupMenu(TPM_RETURNCMD | TPM_NONOTIFY,
				point.x, point.y, this);
			switch (response)
			{
			case 1:
				{
					CSharedFile fText(GMEM_DDESHARE | GMEM_MOVEABLE | GMEM_ZEROINIT);
					CDBRow::WriteString(&fText, pszURL);
					COleDataSource *pDataSource = new COleDataSource;
					pDataSource->CacheGlobalData(CF_TEXT, fText.Detach());
					pDataSource->SetClipboard();
					COleDataSource::FlushClipboard();
				}
				break;
			}
		}
	}
}
