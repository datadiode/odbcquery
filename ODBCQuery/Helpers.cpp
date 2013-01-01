#include "stdafx.h"
#include <afxpriv.h>
#include "Helpers.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CHelpers

BOOL CHelpers::Open(CFile &file, LPCTSTR lpszFileName, UINT nOpenFlags)
{
	CFileException e;
	if (file.Open(lpszFileName, nOpenFlags, &e))
		return TRUE;
	TCHAR szError[1024];
	if (!e.GetErrorMessage(szError, 1024))
		AfxLoadString(AFX_IDP_NO_ERROR_AVAILABLE, szError, 1024);
	CString sMsg;
	sMsg.Format(_T("%s:\n%s"), lpszFileName, szError);
	AfxMessageBox(sMsg, MB_ICONSTOP);
	return FALSE;
}

struct CHelpers::rgDispinfoText CHelpers::rgDispinfoText;

/**
 * @brief Allocate a text buffer to assign to NMLVDISPINFO::item::pszText
 * Quoting from SDK Docs:
 *	If the LVITEM structure is receiving item text, the pszText and cchTextMax
 *	members specify the address and size of a buffer. You can either copy text to
 *	the buffer or assign the address of a string to the pszText member. In the
 *	latter case, you must not change or delete the string until the corresponding
 *	item text is deleted or two additional LVN_GETDISPINFO messages have been sent.
 */
LPTSTR CHelpers::AllocDispinfoText(const CString &s)
{
	static int i = 0;
	LPCTSTR pszText = rgDispinfoText[i] = s;
	i ^= 1;
	return (LPTSTR)pszText;
}

//////////////////////////////////////////////////////////////////////
// CSortListCtrl

CSortListCtrl *CSortListCtrl::From(NMHDR *pNMHDR)
{
	CSortListCtrl *pLv = static_cast<CSortListCtrl *>(FromHandlePermanent(pNMHDR->hwndFrom));
	int nCol = reinterpret_cast<NM_LISTVIEW *>(pNMHDR)->iSubItem;
	int xCol = pLv->m_nModalResult >= 0 ? nCol : ~nCol;
	pLv->m_nModalResult = pLv->m_nModalResult != xCol ? xCol : ~xCol;
	return pLv;
}

BOOL CSortListCtrl::Sort(PFNLVCOMPARE pfnCompare)
{
	return SortItems(pfnCompare, m_nModalResult);
}

BOOL CSortListCtrl::Sort()
{
	int nCol = m_nModalResult >= 0 ? m_nModalResult : ~m_nModalResult;
	int n = GetItemCount();
	int i = 0;
	while (i < n)
	{
		LPTSTR pszText = (LPTSTR)(LPCTSTR)CString();
		reinterpret_cast<CString &>(pszText) = GetItemText(i, nCol);
		reinterpret_cast<CStringData *>(pszText)[-1].nRefs = GetItemData(i);
		SetItemData(i, reinterpret_cast<DWORD>(pszText));
		++i;
	}
	BOOL f = SortItems(Compare, m_nModalResult);
	while (i)
	{
		--i;
		LPTSTR pszText = (LPTSTR)GetItemData(i);
		SetItemData(i, reinterpret_cast<CStringData *>(pszText)[-1].nRefs);
		reinterpret_cast<CStringData *>(pszText)[-1].nRefs = 1;
		reinterpret_cast<CString &>(pszText).~CString();
	}
	return f;
}

int CALLBACK CSortListCtrl::Compare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	int cmp = lstrcmpi((LPCTSTR)lParam1, (LPCTSTR)lParam2);
	return lParamSort >= 0 ? cmp : -cmp;
}
