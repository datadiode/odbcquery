#include "stdafx.h"
#include "resource.h"
#include "DatabaseEx.h"
#include "GenSQLDlg.h"
#include "dbrow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CGenSQLDlg 


CGenSQLDlg::CGenSQLDlg(CDatabaseEx *pDatabase, CListCtrl *pLvResults)
	: CDialog(CGenSQLDlg::IDD)
	, m_pDatabase(pDatabase)
	, m_pLvResults(pLvResults)
{
}

void CGenSQLDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGenSQLDlg)
	DDX_Control(pDX, IDC_LV_COLUMNS, m_LvColumns);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGenSQLDlg, CDialog)
	//{{AFX_MSG_MAP(CGenSQLDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CGenSQLDlg 

BOOL CGenSQLDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CHeaderCtrl *pHdColums = static_cast<CHeaderCtrl *>(CWnd::FromHandle(
		reinterpret_cast<HWND>(m_pLvResults->SendMessage(LVM_GETHEADER))));
	int nColumns = pHdColums->GetItemCount();
	TCHAR szText[MAX_PATH];
	HDITEM hdi;
	LVITEM lvi;
	hdi.pszText = lvi.pszText = szText;
	hdi.cchTextMax = lvi.cchTextMax = MAX_PATH;
	hdi.mask = HDI_TEXT | HDI_LPARAM;
	lvi.mask = LVIF_TEXT | LVIF_PARAM | LVIF_STATE;
	lvi.state = lvi.stateMask = LVIS_SELECTED;
	UINT quot = BST_UNCHECKED;
	for (lvi.iItem = 0 ; lvi.iItem < nColumns ; ++lvi.iItem)
	{
		pHdColums->GetItem(lvi.iItem, &hdi);
		lvi.iSubItem = 0;
		lvi.lParam = hdi.lParam;
		m_LvColumns.InsertItem(&lvi);
		if (StrChr(szText, ' '))
			quot = BST_CHECKED;
	}
	CheckDlgButton(IDC_TG_QUOTED_IDENTIFIERS, quot);
	return TRUE;
}

void CGenSQLDlg::WriteFile(CFile &f)
{
	static const TCHAR sqlFinish[] = { ')', ';', '\r', '\n' };
	CString sTable, sqlInsert;
	m_pLvResults->GetWindowText(sTable);
	sTable.GetBufferSetLength(sTable.ReverseFind(m_pDatabase->m_chIDQuoteChar) + 1);
	int quot = 1;
	if (!IsDlgButtonChecked(IDC_TG_INCLUDE_OWNER))
	{
		int i = sTable.Find(m_pDatabase->m_chIDQuoteChar, 1) + 2;
		if (i <=  sTable.GetLength())
			sTable.Delete(0, i);
	}
	if (!IsDlgButtonChecked(IDC_TG_QUOTED_IDENTIFIERS))
	{
		quot = 0;
		sTable.Remove(m_pDatabase->m_chIDQuoteChar);
	}
	sqlInsert.Format(_T("INSERT INTO %s"), sTable);
	TCHAR szText[4 + MAX_PATH];
	szText[0] = ' ';
	szText[1] = '(';
	szText[2] = '"';
	int i = -1;
	while ((i = m_LvColumns.GetNextItem(i, LVNI_SELECTED)) != -1)
	{
		int len = m_LvColumns.GetItemText(i, 0, szText + 2 + quot, MAX_PATH);
		szText[2 + quot + len] = '"';
		szText[2 + quot + len + quot] = '\0';
		sqlInsert += szText;
		szText[0] = ',';
		szText[1] = ' ';
	}
	sqlInsert += _T(") VALUES");
	szText[2] = '\0';
	int j = -1;
	CDBRow::Handle *pRow;
	while ((j = m_pLvResults->GetNextItem(j, LVNI_SELECTED)) != -1 &&
		(pRow = reinterpret_cast<CDBRow::Handle *>(m_pLvResults->GetItemData(j))) != 0)
	{
		f.Write(sqlInsert, sqlInsert.GetLength());
		szText[0] = ' ';
		szText[1] = '(';
		int i = -1;
		while ((i = m_LvColumns.GetNextItem(i, LVNI_SELECTED)) != -1)
		{
			f.Write(szText, 2);
			CString s = pRow->Item(i)->asString(_T("#ERR"), CDBRow::Item::fEnquoted);
			// Convert a unicode string to octets
			if (int n = s.GetLength())
				if (s.GetAt(n - 1) == 0)
					s = CString((LPCWSTR)&*s, n / 2);
			f.Write(s, s.GetLength());
			szText[0] = ',';
			szText[1] = ' ';
		}
		f.Write(sqlFinish, sizeof sqlFinish);
	}
}

void CGenSQLDlg::OnOK() 
{
	if (IsDlgButtonChecked(IDC_TG_CLIPBOARD))
	{
		CSharedFile f;
		WriteFile(f);
		f.Write(_T(""), sizeof(TCHAR));
		COleDataSource *pDataSource = new COleDataSource;
		pDataSource->CacheGlobalData(sizeof(TCHAR) == 1 ? CF_TEXT : CF_UNICODETEXT, f.Detach());
		pDataSource->SetClipboard();
		COleDataSource::FlushClipboard();
	}
	else
	{
		CString sTable, s;
		m_pLvResults->GetWindowText(s);
		m_pDatabase->ExtractTableName(sTable, s);
		CFileDialog dlg(FALSE, _T("sql"), sTable,
			OFN_OVERWRITEPROMPT,
			_T("SQL Files (*.sql;*.tsq;*.ssq)|*.sql;*.tsq;*.ssq|")
			_T("All Files (*.*)|*.*|")
			_T("|"));
		if (dlg.DoModal() == IDCANCEL)
			return;
		CFile f(dlg.GetPathName(), CFile::modeWrite | CFile::modeCreate);
		WriteFile(f);
	}
	CDialog::OnOK();
}
