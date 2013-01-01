/*/CellEditor.cpp

Last edit: 2013-01-01 Jochen Neubeck

[The MIT license]

Copyright (c) 2008 Jochen Neubeck

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include "stdafx.h"
#include "CellEditor.h"

const TCHAR CellEditor_EditClassName[] = _T("RichEdit20A");

HWND NTAPI CellEditor_CreateEdit(HWND hWnd)
{
	HWND hwndEdit = ::CreateWindow(CellEditor_EditClassName, 0,
		WS_CHILD|ES_MULTILINE|ES_NOHIDESEL|ES_AUTOHSCROLL|WS_CLIPSIBLINGS,
		0, 0, 0, 0, hWnd, 0, 0, 0);
	::SendMessage(hwndEdit, EM_SETTEXTMODE, TM_RICHTEXT|TM_SINGLELEVELUNDO|TM_MULTICODEPAGE, 0);
	WPARAM wFont = ::SendMessage(hWnd, WM_GETFONT, 0, 0);
	::SendMessage(hwndEdit, WM_SETFONT, wFont, 0);
	::SendMessage(hwndEdit, EM_SETBKGNDCOLOR, 0, ::GetSysColor(COLOR_BTNFACE));
	return hwndEdit;
}

BSTR NTAPI CellEditor_GetCellText(HWND hLv, int row, int col)
{
	int cch;
	int len = 128;
	BSTR bstr = ::SysAllocStringLen(0, len);
	do
	{
		LVITEMW lvi;
		lvi.iSubItem = col;
		lvi.cchTextMax = len + 1;
		lvi.pszText = bstr;
		cch = SNDMSG(hLv, LVM_GETITEMTEXTW, row, (LPARAM)&lvi);
	} while (cch >= len && ::SysReAllocStringLen(&bstr, 0, len *= 2));
	::SysReAllocStringLen(&bstr, bstr, cch);
	return bstr;
}


static void NTAPI CellEditor_InjectText(HWND hLv, HWND hwndEdit, int row, int col)
{
	if (BSTR bstr = CellEditor_GetCellText(hLv, row, col))
	{
		LRESULT eventMask = ::SendMessage(hwndEdit, EM_GETEVENTMASK, 0, 0);
		::SendMessage(hwndEdit, EM_SETEVENTMASK, 0, eventMask & ~ENM_CHANGE);
		LVCOLUMN lvc;
		lvc.mask = LVCF_FMT;
		ListView_GetColumn(hLv, col, &lvc);
		PARAFORMAT pf;
		pf.cbSize = sizeof pf;
		pf.dwMask = PFM_ALIGNMENT;
		pf.wAlignment = (lvc.fmt & LVCFMT_JUSTIFYMASK) + 1;
		::SendMessage(hwndEdit, EM_SETPARAFORMAT, 0, (LPARAM)&pf);
		SETTEXTEX settextex;
		settextex.flags = 0;
		settextex.codepage = 1200; // This should also disable RTF parsing...
		/*/ Silly test code to verify assumptions about RTF parsing
		if (StrCmpW(bstr, L"A") == 0)
		{
			SysFreeString(bstr);
			static const char rtf[] = "{\\rtf1{\\i Any braces around?}}";
			bstr = SysAllocStringByteLen(rtf, sizeof rtf - 1);
			settextex.codepage = CP_ACP;
		}
		else if (StrCmpW(bstr, L"W") == 0)
		{
			SysFreeString(bstr);
			static const WCHAR rtf[] = L"{\\rtf1{\\i Any braces around?}}";
			bstr = SysAllocString(rtf);
		}
		/**/
		::SendMessage(hwndEdit, EM_SETTEXTEX, (WPARAM)&settextex, (LPARAM)bstr);
		::SendMessage(hwndEdit, EM_SETEVENTMASK, 0, eventMask | ENM_CHANGE);
		::SysFreeString(bstr);
		::SendMessage(hwndEdit, EM_SETSEL, 0, ::GetWindowTextLength(hwndEdit));
	}
}

static void NTAPI CellEditor_UpdateText(HWND hLv, HWND hwndEdit, int row, int col)
{
	GETTEXTLENGTHEX gettextlengthex;
	gettextlengthex.flags = GT_DEFAULT;
	gettextlengthex.codepage = 1200;
	int cchWideChar = ::SendMessage(hwndEdit, EM_GETTEXTLENGTHEX, (WPARAM)&gettextlengthex, 0);
	GETTEXTEX gettextex;
	gettextex.cb = 2 * (cchWideChar + 1);
	gettextex.flags = GT_DEFAULT;
	gettextex.codepage = 1200;
	gettextex.lpDefaultChar = 0;
	gettextex.lpUsedDefChar = 0;
	if (BSTR bstr = ::SysAllocStringLen(0, cchWideChar))
	{
		::SendMessage(hwndEdit, EM_GETTEXTEX, (WPARAM)&gettextex, (LPARAM)bstr);
		LVITEMW lvi;
		lvi.iSubItem = col;
		lvi.cchTextMax = cchWideChar;
		lvi.pszText = bstr;
		SNDMSG(hLv, LVM_SETITEMTEXTW, row, (LPARAM)&lvi);
		::SysFreeString(bstr);
	}
}

static void NTAPI CellEditor_UpdateRect(HWND hLv, HWND hwndEdit, int row, int col)
{
	WINDOWPLACEMENT wp;
	wp.length = sizeof wp;
	wp.flags = 0;
	wp.showCmd = SW_SHOW;
	ListView_GetSubItemRect(hLv, row, col, LVIR_LABEL, &wp.rcNormalPosition);
	wp.rcNormalPosition.right -= 1;
	wp.rcNormalPosition.bottom -= 1;
	::SetWindowPlacement(hwndEdit, &wp);
	::GetClientRect(hwndEdit, &wp.rcNormalPosition);
	if (col)
	{
		wp.rcNormalPosition.left += 4;
		wp.rcNormalPosition.right -= 4;
	}
	wp.rcNormalPosition.left += 2;
	::SendMessage(hwndEdit, EM_SETRECT, 0, (LPARAM)&wp.rcNormalPosition);
}

static void NTAPI CellEditor_BottomLine(HWND hWnd, HWND hwndEdit)
{
	int row = ListView_GetNextItem(hWnd, -1, LVNI_FOCUSED);
	LVITEMW lvi;
	lvi.iItem = row;
	lvi.iSubItem = 0;
	lvi.mask = LVIF_PARAM;
	SNDMSG(hWnd, LVM_GETITEMW, 0, (LPARAM)&lvi);
	if (lvi.lParam == 0)
	{
		int col = ::GetDlgCtrlID(hwndEdit);
		HWND hwndHead = ListView_GetHeader(hWnd);
		lvi.cchTextMax = ::GetWindowTextLength(hwndEdit);
		lvi.iSubItem = Header_GetItemCount(hwndHead);
		while (lvi.cchTextMax == 0 && lvi.iSubItem)
		{
			if (--lvi.iSubItem != col)
			{
				WCHAR sz[2];
				lvi.pszText = sz;
				lvi.cchTextMax = 2;
				lvi.cchTextMax = SNDMSG(hWnd, LVM_GETITEMTEXTW, lvi.iItem, (LPARAM)&lvi);
			}
		}
		++lvi.iItem;
		if (lvi.cchTextMax == 0)
		{
			::ShowWindow(hwndEdit, SW_HIDE);
			ListView_DeleteItem(hWnd, lvi.iItem);
			CellEditor_UpdateRect(hWnd, hwndEdit, row, col);
			::ShowWindow(hwndEdit, SW_SHOW);
		}
		else if (lvi.iItem == ListView_GetItemCount(hWnd))
		{
			lvi.mask = LVIF_PARAM | LVIF_TEXT;
			lvi.pszText = LPSTR_TEXTCALLBACKW;
			lvi.iSubItem = 0;
			SNDMSG(hWnd, LVM_INSERTITEMW, 0, (LPARAM)&lvi);
		}
	}
}

HWND NTAPI CellEditor_ShowEdit(HWND hLv, BOOL fShow)
{
	HWND hwndEdit = ::FindWindowEx(hLv, 0, CellEditor_EditClassName, 0);
	if (hwndEdit)
	{
		int row = ListView_GetNextItem(hLv, -1, LVNI_FOCUSED);
		int col = ::GetDlgCtrlID(hwndEdit);
		LONG style = ::GetWindowLong(hwndEdit, GWL_STYLE);
		if (row == -1)
		{
			if (::ShowWindow(hwndEdit, SW_HIDE))
				::SendMessage(hwndEdit, WM_KILLFOCUS, 0, 0);
			::SetWindowText(hwndEdit, _T(""));
		}
		else if (fShow)
		{
			CellEditor_UpdateRect(hLv, hwndEdit, row, col);
			if ((style & WS_VISIBLE) == 0)
				::SendMessage(hwndEdit, WM_SETFOCUS, 0, 0);
			if (::SendMessage(hwndEdit, EM_GETMODIFY, 0, 0))
			{
				LVITEMW lvi;
				lvi.iSubItem = col;
				lvi.pszText = LPSTR_TEXTCALLBACKW;
				SNDMSG(hLv, LVM_SETITEMTEXTW, row, (LPARAM)&lvi);
			}
			else
			{
				CellEditor_InjectText(hLv, hwndEdit, row, col);
			}
		}
		else if (::ShowWindow(hwndEdit, SW_HIDE))
		{
			LRESULT fModify = ::SendMessage(hwndEdit, EM_GETMODIFY, 0, 0);
			::SendMessage(hwndEdit, WM_KILLFOCUS, 0, 0);
			if (fModify)
			{
				CellEditor_UpdateText(hLv, hwndEdit, row, col);
				if (::SendMessage(hwndEdit, EM_GETMODIFY, 0, 0))
					::SetWindowText(hwndEdit, _T(""));
				else
					::SendMessage(hwndEdit, EM_SETMODIFY, 1, 0);
			}
			else
			{
				::SetWindowText(hwndEdit, _T(""));
			}
		}
	}
	return hwndEdit;
}

static void NTAPI CellEditor_EnsureVisible(HWND hWnd, int row, int col)
{
	ListView_EnsureVisible(hWnd, row, FALSE);
	RECT rcOuter, rcInner;
	::GetClientRect(hWnd, &rcOuter);
	ListView_GetSubItemRect(hWnd, row, col, LVIR_LABEL, &rcInner);
	if (col == 0)
		rcInner.left = -::GetScrollPos(hWnd, SB_HORZ);
	int dx = rcInner.left;
	if (rcInner.right > rcOuter.right)
		::OffsetRect(&rcInner, rcOuter.right - rcInner.right, 0);
	if (rcInner.left < rcOuter.left)
		::OffsetRect(&rcInner, rcOuter.left - rcInner.left, 0);
	if (dx -= rcInner.left)
		ListView_Scroll(hWnd, dx, 0);
}

void NTAPI CellEditor_SelectCell(HWND hWnd, int row, int col)
{
	if (HWND hwndEdit = ::FindWindowEx(hWnd, 0, CellEditor_EditClassName, 0))
	{
		if (row >= 0 && row < ListView_GetItemCount(hWnd))
		{
			if (ListView_GetItemState(hWnd, row, LVIS_FOCUSED) & LVIS_FOCUSED)
			{
				CellEditor_ShowEdit(hWnd, FALSE);
				if (::SendMessage(hwndEdit, EM_GETMODIFY, 0, 0) == 0)
				{
					::SetWindowLong(hwndEdit, GWL_ID, col);
				}
				else
				{
					row = -1;
				}
				::PostMessage(hWnd, WM_VSCROLL, SB_ENDSCROLL, 0);
			}
			else
			{
				LVITEMW lvi;
				lvi.mask = LVIF_STATE;
				lvi.stateMask = LVIS_FOCUSED;
				lvi.state = LVIS_FOCUSED;
				SNDMSG(hWnd, LVM_SETITEMSTATE, row, (LPARAM)&lvi);
				if (ListView_GetItemState(hWnd, row, LVIS_FOCUSED) & LVIS_FOCUSED)
				{
					::SetWindowLong(hwndEdit, GWL_ID, col);
					lvi.stateMask = LVIS_SELECTED;
					lvi.state = 0;
					SNDMSG(hWnd, LVM_SETITEMSTATE, -1, (LPARAM)&lvi);
					ListView_SetSelectionMark(hWnd, row);
					lvi.state = LVIS_SELECTED;
					SNDMSG(hWnd, LVM_SETITEMSTATE, row, (LPARAM)&lvi);
				}
				else
				{
					row = -1;
				}
			}
			if (row != -1)
			{
				CellEditor_EnsureVisible(hWnd, row, col);
			}
		}
	}
}

static UINT NTAPI CellEditor_GetModifiers()
{
	UINT fsModifiers = 0;
	if (::GetKeyState(VK_CONTROL) < 0)
		fsModifiers |= MOD_CONTROL;
	if (::GetKeyState(VK_SHIFT) < 0)
		fsModifiers |= MOD_SHIFT;
	if (::GetKeyState(VK_MENU) < 0)
		fsModifiers |= MOD_ALT;
	return fsModifiers;
}

static UINT NTAPI CellEditor_IsClipboardShortcut(WPARAM wParam, UINT fsModifiers)
{
	// Leave Ctrl+Shift combinations to row-level clipboard actions
	switch (fsModifiers & (MOD_CONTROL | MOD_SHIFT))
	{
	case MOD_CONTROL:
		switch (wParam)
		{
		case 'X':
			return WM_CUT;
		case 'C': case VK_INSERT:
			return WM_COPY;
		case 'V':
			return WM_PASTE;
		}
		break;
	case MOD_SHIFT:
		switch (wParam)
		{
		case VK_INSERT:
			return WM_PASTE;
		}
		break;
	}
	return 0;
}

static WNDPROC CellEditor_DefWndProc = 0;

static LRESULT CALLBACK CellEditor_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	WNDPROC DefWndProc = CellEditor_DefWndProc;
	switch (uMsg)
	{
	case WM_GETDLGCODE:
		if (MSG *pMsg = (MSG *)lParam)
		{
			switch (pMsg->message)
			{
			case WM_KEYDOWN:
				switch (pMsg->wParam)
				{
				case VK_ESCAPE:
					return DLGC_WANTMESSAGE;
				}
				break;
			}
		}
		break;
	case WM_KILLFOCUS:
		CellEditor_ShowEdit(hWnd, FALSE);
		::SetWindowLong(hWnd, GWL_WNDPROC, (LONG)CellEditor_DefWndProc);
		::SetWindowLong(hWnd, GWL_STYLE, ::GetWindowLong(hWnd, GWL_STYLE) | LVS_EDITLABELS);
		CellEditor_DefWndProc = 0;
		break;
	case WM_HSCROLL:
	case WM_VSCROLL:
		CellEditor_ShowEdit(hWnd, LOWORD(wParam) == SB_ENDSCROLL);
		break;
	case WM_NOTIFY:
		switch (((NMHDR *)lParam)->code)
		{
		case HDN_ENDTRACKA:
		case HDN_ENDTRACKW:
		case HDN_DIVIDERDBLCLICKA:
		case HDN_DIVIDERDBLCLICKW:
		case HDN_ITEMCLICKA:
		case HDN_ITEMCLICKW:
			::PostMessage(hWnd, WM_VSCROLL, SB_ENDSCROLL, 0);
			// fall through
		case HDN_BEGINTRACKA:
		case HDN_BEGINTRACKW:
			CellEditor_ShowEdit(hWnd, FALSE);
			break;
		}
		break;
	case WM_MOUSEWHEEL:
		::PostMessage(hWnd, WM_VSCROLL, SB_ENDSCROLL, 0);
		CellEditor_ShowEdit(hWnd, FALSE);
		break;
	case WM_LBUTTONDOWN:
		if (HWND hwndEdit = ::FindWindowEx(hWnd, 0, CellEditor_EditClassName, 0))
		{
			LVHITTESTINFO hittest;
			POINTSTOPOINT(hittest.pt, lParam);
			int iItem = ListView_HitTest(hWnd, &hittest);
			if (iItem == -1 || (ListView_GetItemState(hWnd, iItem, LVIS_FOCUSED) & LVIS_FOCUSED) == 0)
			{
				ListView_SetItemState(hWnd, -1, 0, LVIS_FOCUSED);
				if (ListView_GetNextItem(hWnd, -1, LVNI_FOCUSED) != -1)
					return 0;
			}
		}
		break;
	case WM_KEYDOWN:
		if (HWND hwndEdit = ::FindWindowEx(hWnd, 0, CellEditor_EditClassName, 0))
		{
			UINT fsModifiers = CellEditor_GetModifiers();
			DWORD lower = 0;
			DWORD upper = 0;
			DWORD total = (DWORD)::GetWindowTextLength(hwndEdit);
			int row = ListView_GetNextItem(hWnd, -1, LVNI_FOCUSED);
			int col = ::GetDlgCtrlID(hwndEdit);
			::SendMessage(hwndEdit, EM_GETSEL, (WPARAM)&lower, (LPARAM)&upper);
			HWND hwndHead = ListView_GetHeader(hWnd);
			switch (wParam)
			{
			case VK_LEFT:
				fsModifiers &= (MOD_CONTROL|MOD_ALT);
				if (fsModifiers == (MOD_CONTROL|MOD_ALT))
				{
					CellEditor_SelectCell(hWnd, row, 0);
					return 0;
				}
				if (fsModifiers == 0 && lower == 0 && (upper == 0 || upper >= total))
				{
					if (--col < 0)
					{
						col += Header_GetItemCount(hwndHead);
						if (--row < 0)
							return 0;
						::SetWindowLong(hwndEdit, GWL_ID, col);
						CellEditor_EnsureVisible(hWnd, row, col);
						wParam = VK_UP;
						break;
					}
					CellEditor_SelectCell(hWnd, row, col);
					return 0;
				}
				if (!::IsWindowEnabled(hwndEdit))
					return 0;
				DefWndProc = (WNDPROC)::GetWindowLong(hWnd = hwndEdit, GWL_WNDPROC);
				break;
			case VK_RIGHT:
				fsModifiers &= (MOD_CONTROL|MOD_ALT);
				if (fsModifiers == (MOD_CONTROL|MOD_ALT))
				{
					CellEditor_SelectCell(hWnd, row, Header_GetItemCount(hwndHead) - 1);
					return 0;
				}
				if (fsModifiers == 0 && upper >= total && (lower == 0 || lower >= total))
				{
					if (++col >= Header_GetItemCount(hwndHead))
					{
						col = 0;
						if (++row >= ListView_GetItemCount(hWnd))
							return 0;
						::SetWindowLong(hwndEdit, GWL_ID, col);
						CellEditor_EnsureVisible(hWnd, row, col);
						wParam = VK_DOWN;
						break;
					}
					CellEditor_SelectCell(hWnd, row, col);
					return 0;
				}
				if (!::IsWindowEnabled(hwndEdit))
					return 0;
				DefWndProc = (WNDPROC)::GetWindowLong(hWnd = hwndEdit, GWL_WNDPROC);
				break;
			case VK_BACK:
			case VK_DELETE:
				if (!::IsWindowEnabled(hwndEdit))
					return 0;
				DefWndProc = (WNDPROC)::GetWindowLong(hWnd = hwndEdit, GWL_WNDPROC);
				break;
			case VK_ESCAPE:
				if (fsModifiers & MOD_SHIFT)
				{
					int count = Header_GetItemCount(hwndHead);
					for (int col = 0 ; col < count ; ++col)
					{
						LVITEMW lvi;
						lvi.iSubItem = col;
						lvi.pszText = LPSTR_TEXTCALLBACKW;
						SNDMSG(hWnd, LVM_SETITEMTEXTW, row, (LPARAM)&lvi);
					}
					::SendMessage(hwndEdit, EM_SETMODIFY, 1, 0);
				}
				if (::SendMessage(hwndEdit, EM_GETMODIFY, 0, 0))
				{
					CellEditor_InjectText(hWnd, hwndEdit, row, col);
					CellEditor_BottomLine(hWnd, hwndEdit);
				}
				break;
			case VK_PRIOR:
				total = 0;
				//fall through
			case VK_NEXT:
				if ((fsModifiers & (MOD_CONTROL|MOD_SHIFT)) == (MOD_CONTROL|MOD_SHIFT))
				{
					::SendMessage(hwndEdit, EM_SETSEL, total, total);
					return 0;
				}
				break;
			default:
				if (UINT uClip = CellEditor_IsClipboardShortcut(wParam, fsModifiers))
				{
					uMsg = uClip;
					wParam = 0;
					lParam = 0;
					if (uMsg == WM_PASTE)
					{
						uMsg = EM_PASTESPECIAL;
						wParam = CF_TEXT;
					}
					DefWndProc = (WNDPROC)::GetWindowLong(hWnd = hwndEdit, GWL_WNDPROC);
				}
				break;
			}
		}
		break;
	case WM_CHAR:
		if (HWND hwndEdit = ::FindWindowEx(hWnd, 0, CellEditor_EditClassName, 0))
		{
			LONG style = ::GetWindowLong(hwndEdit, GWL_STYLE);
			UINT fsModifiers = CellEditor_GetModifiers();
			if (fsModifiers & MOD_CONTROL)
			{
				if (wParam == VK_SPACE)
					break;
			}
			else
			{
				if (style & WS_DISABLED)
					return 0;
				if ((style & ES_NUMBER) && (wParam < '0' || wParam > '9'))
					return 0;
			}
			DefWndProc = (WNDPROC)::GetWindowLong(hWnd = hwndEdit, GWL_WNDPROC);
		}
		break;
	case WM_COMMAND:
		switch HIWORD(wParam)
		{
		case EN_CHANGE:
			if (::SendMessage((HWND)lParam, EM_GETMODIFY, 0, 0))
			{
				int row = ListView_GetNextItem(hWnd, -1, LVNI_FOCUSED);
				int col = ::GetDlgCtrlID((HWND)lParam);
				CellEditor_EnsureVisible(hWnd, row, col);
				CellEditor_UpdateRect(hWnd, (HWND)lParam, row, col);
				CellEditor_BottomLine(hWnd, (HWND)lParam);
			}
			break;
		case EN_SETFOCUS:
		case EN_KILLFOCUS:
			::SendMessage(::GetParent(hWnd), WM_COMMAND,
				MAKEWPARAM(::GetDlgCtrlID(hWnd), HIWORD(wParam)), lParam);
			break;
		}
		break;
	}
	return ::CallWindowProc(DefWndProc, hWnd, uMsg, wParam, lParam);
}

void NTAPI CellEditor_SubclassListView(HWND hWnd)
{
	ASSERT(CellEditor_DefWndProc == 0);
	CellEditor_DefWndProc = (WNDPROC)::SetWindowLong(hWnd, GWL_WNDPROC, (LONG)CellEditor_WndProc);
}
