#include "stdafx.h"
#include "resource.h"
#include "helpers.h"
#include "DBRow.h"
#include "ODBCQuery.h"
#include "ODBCQueryDoc.h"
#include "ChildFrm.h"
#include "CellEditor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CODBCQueryDoc

IMPLEMENT_DYNCREATE(CODBCQueryDoc, CScintillaDoc)

BEGIN_MESSAGE_MAP(CODBCQueryDoc, CScintillaDoc)
	//{{AFX_MSG_MAP(CODBCQueryDoc)
	ON_COMMAND(ID_QUERY_EXECUTE, OnQueryExecute)
	ON_UPDATE_COMMAND_UI(ID_QUERY_EXECUTE, OnUpdateQueryExecute)
	//}}AFX_MSG_MAP
	//ON_COMMAND(ID_HELP_ODBC_INFO, OnHelpOdbcInfo)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CODBCQueryDoc Konstruktion/Destruktion

CODBCQueryDoc::CODBCQueryDoc()
{
}

CODBCQueryDoc::~CODBCQueryDoc()
{
}

CScintillaView *CODBCQueryDoc::NewScintillaView()
{
	return new CScintillaView;
}

CRecordsetEx *CODBCQueryDoc::GetCurrentRecordset()
{
	CDatabaseEx *pdb = static_cast<CDatabaseEx *>(&m_db);
	if (pdb->m_listRecordsets.IsEmpty())
		return 0;
	return reinterpret_cast<CRecordsetEx *>(pdb->m_listRecordsets.GetHead());
}

BOOL CODBCQueryDoc::SaveModified()
{
	return TRUE;
}

BOOL CODBCQueryDoc::CancelQuery()
{
	BOOL bCancel = FALSE;
	while (CRecordsetEx *prs = GetCurrentRecordset())
	{
		prs->Release();
		bCancel = TRUE;
	}
	if (bCancel)
	{
		CChildFrame *pFrame = static_cast<CChildFrame *>(GetView()->GetParentFrame());
		pFrame->AppendLog(_T("user break"));
	}
	return bCancel;
}

void CODBCQueryDoc::OnQueryExecute() 
{
	if (CancelQuery())
		return;
	CChildFrame *pFrame = static_cast<CChildFrame *>(GetView()->GetParentFrame());
	if (!pFrame->ClearResults())
		return;
	CScintillaCtrl &edit = GetView()->GetCtrl();
	CString strSQL = edit.GetSelText();
	if (strSQL.IsEmpty())
		edit.GetWindowText(strSQL);
	LPCTSTR pchSQL = strSQL;
	static const TCHAR szNoise[] = _T("; \t\r\n");
	while (int cchSQL = CRecordsetEx::LookAheadSQL(pchSQL += StrSpn(pchSQL, szNoise)))
	{
		CRecordsetEx *prs = CRecordsetEx::New(&m_db);
		if (cchSQL > 0)
		{
			prs->m_strSQL = CString(pchSQL, cchSQL);
		}
		else
		{
			cchSQL = -cchSQL;
			prs->m_strSQL = CString(pchSQL + 1, cchSQL - 2);
		}
		pchSQL += cchSQL;
	}
}

void CODBCQueryDoc::OnUpdateQueryExecute(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(GetCurrentRecordset() != 0);
}

CListCtrl *CODBCQueryDoc::CreateListCtrl(CRecordsetEx *prs, const RECT &rect, CWnd *pFrame, UINT nID)
{
	CODBCQueryDocTemplate *pDocTemplate = static_cast<CODBCQueryDocTemplate *>(GetDocTemplate());
	CSortListCtrl *pLv = static_cast<CSortListCtrl *>(new CListCtrl);
	pLv->Create(WS_CHILD|WS_BORDER|LVS_REPORT|LVS_SHOWSELALWAYS, rect, pFrame, nID);
	pLv->SetWindowText(prs->m_strTableName);
	pLv->SendMessage(CCM_SETUNICODEFORMAT, TRUE);
	//pLv->SetExtendedStyle(LVS_EX_FULLROWSELECT);
	ListView_SetExtendedListViewStyle(pLv->m_hWnd, LVS_EX_FULLROWSELECT);
	HWND hHd = ListView_GetHeader(pLv->m_hWnd);
	Header_SetImageList(hHd, pDocTemplate->m_imagelist.m_hImageList);
	int cxPrecision = pLv->GetStringWidth(_T("m"));
	int cxScale = pLv->GetStringWidth(_T("."));
	short nCols = prs->GetODBCFieldCount();
	const UDWORD nMaxWidth = 50;
	LVCOLUMN lvc;
	lvc.mask = 0;
	pLv->InsertColumn(0, &lvc);
	const int cxExtra = 12;
	short n = 0;
	CODBCFieldInfo *rgODBCFieldInfos = new CODBCFieldInfo[nCols];
	::SetWindowLong(pLv->m_hWnd, GWL_USERDATA, (LONG)rgODBCFieldInfos);
	while (n < nCols)
	{
		CODBCFieldInfo &fieldInfo = rgODBCFieldInfos[n];
		prs->GetODBCFieldInfo(n, fieldInfo);
		UDWORD nPrecision = fieldInfo.m_nPrecision;
		lvc.mask = LVCF_TEXT|LVCF_WIDTH|LVCF_FMT|LVCF_IMAGE;
		lvc.fmt = LVCFMT_BITMAP_ON_RIGHT|LVCFMT_IMAGE;
		int cxHeader = cxExtra + 28;
		if (fieldInfo.m_nNullability & CRecordsetEx::nNullabilityPK)
		{
			lvc.iImage = CODBCQueryDocTemplate::iImagePK;
		}
		else
		{
			lvc.fmt = 0;
			lvc.mask &= ~LVCF_IMAGE;
			cxHeader = cxExtra;
		}
		cxHeader += pLv->GetStringWidth(fieldInfo.m_strName);
		switch (fieldInfo.m_nSQLType)
		{
		case SQL_DATE:
		case SQL_TIME:
		case SQL_TIMESTAMP:
			nPrecision = 17;
			break;
		case SQL_LONGVARCHAR:
		case SQL_WLONGVARCHAR:
		case SQL_BINARY:
		case SQL_VARBINARY:
		case SQL_LONGVARBINARY:
			nPrecision *= 2;
			break;
		case SQL_NUMERIC:
		case SQL_DECIMAL:
		case SQL_INTEGER:
		case SQL_SMALLINT:
		case SQL_TINYINT:
		case SQL_FLOAT:
		case SQL_REAL:
		case SQL_DOUBLE:
			lvc.fmt |= LVCFMT_RIGHT;
			break;
		}
		if (nPrecision > nMaxWidth)
			nPrecision = nMaxWidth;
		lvc.cx = nPrecision * cxPrecision + cxExtra;
		if (fieldInfo.m_nScale)
			lvc.cx += cxScale;
		if (lvc.cx < cxHeader)
			lvc.cx = cxHeader;
		lvc.pszText = const_cast<LPTSTR>(&*fieldInfo.m_strName);
		pLv->InsertColumn(++n, &lvc);
	}
	pLv->DeleteColumn(0);
	pLv->m_nModalResult = INT_MAX;
	return pLv;
}

static CString GetColumnNames(CRecordset *prs)
{
	CString sColumns;
	CODBCFieldInfo fieldInfo;
	TCHAR chIDQuoteChar = prs->m_pDatabase->m_chIDQuoteChar;
	short nCols = prs->GetODBCFieldCount();
	for (short n = 0 ; n < nCols ; ++n)
	{
		prs->GetODBCFieldInfo(n, fieldInfo);
		if (sColumns.GetLength())
			sColumns += _T(", ");
		sColumns += chIDQuoteChar + fieldInfo.m_strName + chIDQuoteChar;
	}
	return sColumns;
}

void CODBCQueryDoc::OnIdle()
{
	CRecordsetEx *prs = GetCurrentRecordset();
	if (prs == 0)
		return;
	CChildFrame *pFrame = static_cast<CChildFrame *>(GetView()->GetParentFrame());
	CScintillaCtrl &edit = GetView()->GetCtrl();
	CString strSQL, sMinor, sText;
	try
	{
		TCHAR szText[1024];
		DWORD dwTicks = GetTickCount();
		int nPage = pFrame->m_TcResults.GetItemCount();
		RECT rect;
		pFrame->m_TcResults.GetClientRect(&rect);
		pFrame->m_TcResults.AdjustRect(FALSE, &rect);
		pFrame->m_TcResults.MapWindowPoints(pFrame, &rect);
		if (CRecordsetEx::ExecuteStatus status = prs->Execute())
		{
			int cchText = edit.GetTextLength();
			if (status == CRecordsetEx::skipped)
			{
				edit.AppendText(prs->m_strSQL.GetLength(), prs->m_strSQL);
				edit.AppendText(2, _T(";\n"));
				edit.SetSel(cchText, -1);
				pFrame->m_strStatus = _T("skipped\n");
				pFrame->AppendLog(pFrame->m_strStatus);
				prs->Release();
			}
			else
			{
				BOOL fForceNewPage = prs->m_strTableName.IsEmpty();
				switch (status)
				{
				case CRecordsetEx::initial:
					fForceNewPage = TRUE;
					// fall through
				case CRecordsetEx::initial_sp_helptext:
					if (prs->m_strTableName.GetLength())
					{
						strSQL = prs->m_strSQL;
						strSQL.Replace(_T("*"), GetColumnNames(prs));
						edit.AppendText(strSQL.GetLength(), strSQL);
						edit.AppendText(2, _T(";\n"));
						edit.SetSel(cchText, -1);
					}
					break;
				case CRecordsetEx::initial_sql:
					fForceNewPage = TRUE;
					edit.AppendText(prs->m_strSQL.GetLength(), prs->m_strSQL);
					edit.AppendText(2, _T(";\n"));
					edit.SetSel(cchText, -1);
					break;
				}
				m_dwTicks = dwTicks;
				short nCols = prs->GetODBCFieldCount();
				if (nCols == 0)
				{
					SQLINTEGER nRowsAffected;
					if SQL_SUCCEEDED(::SQLRowCount(prs->m_hstmt, &nRowsAffected))
					{
						pFrame->m_strStatus.Format(_T("%d row%s affected\n"),
							nRowsAffected, &_T("\0s")[nRowsAffected != 1]);
					}
					else
					{
						pFrame->m_strStatus = _T("done\n");
					}
					pFrame->AppendLog(pFrame->m_strStatus);
					prs->Release();
					if (status == CRecordsetEx::initial_use)
					{
						BeginWaitCursor();
						pFrame->PopulateCbUse();
						pFrame->PopulateTcDictionary();
						EndWaitCursor();
					}
				}
				else
				{
					CDatabaseEx *pdb = static_cast<CDatabaseEx *>(prs->m_pDatabase);
					bool bIsTable = pdb->ExtractTableName(sMinor, prs->m_strTableName);
					if (fForceNewPage || nPage == 1 || bIsTable)
					{
						TCITEM tci;
						tci.cchTextMax = 0;
						wsprintf(tci.pszText = szText, _T("#%d"), nPage);
						if (status == CRecordsetEx::initial_sp_helptext)
						{
							CScintillaCtrl *pEd = new CScintillaCtrl;
							pEd->Create(WS_CHILD|WS_BORDER, rect, pFrame, 2020);
							pFrame->LoadStyles(*pEd);
							pEd->SetReadOnly(TRUE);
							tci.mask = TCIF_TEXT|TCIF_PARAM;
							tci.lParam = reinterpret_cast<LPARAM>(pEd);
						}
						else
						{
							CListCtrl *pLv = CreateListCtrl(prs, rect, pFrame, 2000 + status);
							if (prs->m_bUpdatable)
							{
								pLv->SetDlgCtrlID(2000);
								CellEditor_CreateEdit(pLv->m_hWnd);
								tci.pszText = const_cast<LPTSTR>(&*sMinor);
							}
							tci.mask = TCIF_TEXT|TCIF_PARAM|TCIF_IMAGE;
							tci.lParam = reinterpret_cast<LPARAM>(pLv);
							tci.iImage = I_IMAGENONE;
						}
						pFrame->m_TcResults.SetWindowPos(
							reinterpret_cast<CWnd *>(tci.lParam),
							0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
						pFrame->m_TcResults.InsertItem(nPage, &tci);

						wsprintf(szText, _T("[#%d]"), nPage);
						pFrame->AppendLog(szText);

						if (pFrame->m_TcResults.GetCurSel() == 0)
						{
							pFrame->m_TcResults.SetCurSel(nPage);
							reinterpret_cast<CWnd *>(tci.lParam)->ShowWindow(SW_SHOW);
							pFrame->m_EdLog.ShowWindow(SW_HIDE);
						}
					}
				}
			}
		}
		else
		{
			TCITEM tci;
			tci.mask = TCIF_PARAM;
			pFrame->m_TcResults.GetItem(nPage - 1, &tci);
			CWnd *pWnd = reinterpret_cast<CWnd *>(tci.lParam);

			LVITEM lvi;

			int nChunk = 50;
			if (CScintillaCtrl *pEd = DYNAMIC_DOWNCAST(CScintillaCtrl, pWnd))
			{
				lvi.iItem = pEd->GetLineCount();
				pEd->SetReadOnly(FALSE);
				while (nChunk && !prs->IsEOF())
				{
					prs->GetFieldValue((short)0, sText);
					pEd->AppendText(sText.GetLength(), sText);
					++lvi.iItem;
					prs->MoveNext();
					--nChunk;
				}
				pEd->SetReadOnly(TRUE);
			}
			else
			{
				CListCtrl *pLv = static_cast<CListCtrl *>(pWnd);
				lvi.mask = LVIF_TEXT|LVIF_PARAM;
				lvi.iItem = pLv->GetItemCount();
				lvi.iSubItem = 0;
				lvi.pszText = LPSTR_TEXTCALLBACK;
				if (lvi.iItem == 0)
				{
					lvi.mask = LVIF_TEXT|LVIF_PARAM|LVIF_STATE;
					lvi.state = lvi.stateMask = LVIS_FOCUSED|LVIS_SELECTED;
				}
				while (nChunk && !prs->IsEOF())
				{
					lvi.lParam = (LPARAM)CDBRow::Scan(*prs);
					pLv->InsertItem(&lvi);
					++lvi.iItem;
					lvi.mask = LVIF_TEXT|LVIF_PARAM;
					prs->MoveNext();
					--nChunk;
				}
			}
			if (prs->IsEOF())
			{
				int nRowsAffected = prs->m_lRecordCount;
				pFrame->m_strStatus.Format(
					_T("%d row%s returned - elapsed time: %lu ms\n"),
					nRowsAffected, &_T("\0s")[nRowsAffected != 1], GetTickCount() - m_dwTicks);
				pFrame->AppendLog(pFrame->m_strStatus);
				if (prs->MoreResults())
				{
					TCITEM tci;
					tci.mask = TCIF_TEXT|TCIF_PARAM;
					tci.pszText = szText;
					tci.cchTextMax = 0;
					wsprintf(szText, _T("#%d"), nPage);

					int nID = pWnd->GetDlgCtrlID();
					if (nID == 2020)
					{
						CScintillaCtrl *pEd = new CScintillaCtrl;
						pEd->Create(WS_CHILD|WS_BORDER, rect, pFrame, nID);
						pFrame->LoadStyles(*pEd);
						pEd->SetReadOnly(TRUE);
						tci.lParam = reinterpret_cast<LPARAM>(pEd);
					}
					else
					{
						CListCtrl *pLv = CreateListCtrl(prs, rect, pFrame, nID);
						tci.lParam = reinterpret_cast<LPARAM>(pLv);
					}
					pFrame->m_TcResults.SetWindowPos(
						reinterpret_cast<CWnd *>(tci.lParam),
						0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
					pFrame->m_TcResults.InsertItem(nPage, &tci);

					wsprintf(szText, _T("[#%d]"), nPage);
					pFrame->AppendLog(szText);

					if (nPage == 1)
					{
						pFrame->m_TcResults.SetCurSel(nPage);
						reinterpret_cast<CWnd *>(tci.lParam)->ShowWindow(SW_SHOW);
						pFrame->m_EdLog.ShowWindow(SW_HIDE);
					}
				}
				else
				{
					prs->Release();
				}
				if (pWnd->GetDlgCtrlID() == 2000 + CRecordsetEx::initial_catalog)
				{
					pWnd->GetWindowText(sText);
					if (sText.Find('.') != -1 || GetCurrentRecordset() == 0)
					{
						pWnd->SetRedraw(FALSE);
						CLvPopulator::AdjustColumns(pWnd->m_hWnd);
						pWnd->SetRedraw(TRUE);
						pWnd->RedrawWindow();
					}
				}
				else if (CListCtrl *pLv = DYNAMIC_DOWNCAST(CListCtrl, pWnd))
				{
					lvi.lParam = 0;
					pLv->InsertItem(&lvi);
				}
			}
			else
			{
				int nRowsAffected = prs->m_lCurrentRecord;
				pFrame->m_strStatus.Format(
					_T("%d row%s processed - elapsed time: %lu ms\n"),
					nRowsAffected, &_T("\0s")[nRowsAffected != 1], GetTickCount() - m_dwTicks);
			}
		}
	}
	catch (CException *e)
	{
		pFrame->AppendLog(e, MB_ICONSTOP);
		prs->Release();
	}
	pFrame->PostMessage(WM_KICKIDLE);
}
