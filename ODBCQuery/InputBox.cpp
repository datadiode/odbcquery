#include "stdafx.h"
#include "InputBox.h"

CInputBox::CInputBox(UINT nIDTemplate, CWnd *pParentWnd)
: CDialog(nIDTemplate, pParentWnd)
{
	CWnd *pTopLevelParentWnd = m_pParentWnd->GetTopLevelParent();
	m_pParentWnd->EnableWindow(FALSE);
	pTopLevelParentWnd->EnableWindow(FALSE);
	Create(MAKEINTRESOURCE(nIDTemplate), pTopLevelParentWnd);
}

CInputBox::~CInputBox()
{
	CWnd *pTopLevelParentWnd = m_pParentWnd->GetTopLevelParent();
	pTopLevelParentWnd->EnableWindow(TRUE);
	m_pParentWnd->EnableWindow(TRUE);
}

BOOL CInputBox::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (HIWORD(wParam) != BN_CLICKED)
		return FALSE;
	EndDialog(LOWORD(wParam));
	return TRUE;
}
