/*/WebLinkButton.cpp

Last edit: 2013-01-01 Jochen Neubeck

[The MIT license]

Copyright (c) 2007 Jochen Neubeck

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
#include "WebLinkButton.h"

EXTERN_C void NTAPI MakeWebLinkButton(HWND hWnd, int nID, LPCTSTR pszURL)
{
	TCHAR szText[1024];
	HWND hwndStatic = ::GetDlgItem(hWnd, nID);
	::GetWindowText(hwndStatic, szText, RTL_NUMBER_OF(szText));
	HDC hDC = ::GetDC(0);
	HFONT hFont = (HFONT)::SendMessage(hwndStatic, WM_GETFONT, 0, 0);
	::SelectObject(hDC, hFont);
	RECT rgrc[2];
	HWND hwndInsertAfter = hwndStatic;
	while (LPTSTR pchLower = StrChr(szText, '['))
	{
		StrTrim(pchLower, _T("["));
		LPTSTR pchUpper = StrChr(pchLower, ']');
		*pchUpper = '\0';
		::ZeroMemory(rgrc, sizeof rgrc);
		::DrawText(hDC, szText, pchLower - szText, &rgrc[0], DT_CALCRECT);
		::DrawText(hDC, szText, pchUpper - szText, &rgrc[1], DT_CALCRECT);
		::MapWindowPoints(hwndStatic, hWnd, (LPPOINT)&rgrc, 4);
		HWND hwndButton = ::CreateWindow(_T("BUTTON"), pchLower,
			WS_CHILD | WS_TABSTOP | WS_VISIBLE | BS_OWNERDRAW | BS_NOTIFY,
			rgrc[0].right, rgrc[0].top,
			rgrc[1].right - rgrc[0].right + 1,
			rgrc[1].bottom - rgrc[0].top + 1,
			hWnd, (HMENU)nID, 0, 0);
		::SendMessage(hwndButton, WM_SETFONT, (WPARAM)hFont, 0);
		*pchUpper = '[';
		StrTrim(pchUpper, _T("["));
		::SetWindowLong(hwndButton, GWL_USERDATA, (LONG)pszURL);
		::SetWindowPos(hwndButton, hwndInsertAfter, 0, 0, 0, 0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
		hwndInsertAfter = hwndButton;
		pszURL += lstrlen(pszURL) + 1;
	}
	::SetWindowText(hwndStatic, szText);
	::ReleaseDC(0, hDC);
}

EXTERN_C void NTAPI DrawWebLinkButton(DRAWITEMSTRUCT *pParam)
{
	RECT rcUnderline =
	{
		pParam->rcItem.left, pParam->rcItem.bottom - 2,
		pParam->rcItem.right, pParam->rcItem.bottom - 1
	};
	TCHAR cText[INTERNET_MAX_PATH_LENGTH];
	int cchText = ::GetWindowText(pParam->hwndItem, cText, INTERNET_MAX_PATH_LENGTH);
	COLORREF clrText = RGB(0,0,255);
	LONG lStyle = ::GetWindowLong(pParam->hwndItem, GWL_STYLE);
	if (lStyle & BS_LEFTTEXT)
	{
		clrText = RGB(128,0,128);
	}
	switch (pParam->itemAction)
	{
	case ODA_DRAWENTIRE:
		::ExtTextOut(pParam->hDC, 0, 0, ETO_OPAQUE, &pParam->rcItem, 0, 0, 0);
		::SetBkMode(pParam->hDC, TRANSPARENT);
		::SetTextColor(pParam->hDC, clrText);
		::DrawText(pParam->hDC, cText, cchText, &pParam->rcItem, DT_LEFT);
		::SetBkColor(pParam->hDC, clrText);
		::ExtTextOut(pParam->hDC, 0, 0, ETO_OPAQUE, &rcUnderline, 0, 0, 0);
		if (pParam->itemState & ODS_FOCUS)
		{
		case ODA_FOCUS:
			if (!(pParam->itemState & ODS_NOFOCUSRECT))
			{
				::SetTextColor(pParam->hDC, 0);
				::SetBkColor(pParam->hDC, RGB(255,255,255));
				::SetBkMode(pParam->hDC, OPAQUE);
				DrawFocusRect(pParam->hDC, &pParam->rcItem);
			}
		}
		break;
	}
}

EXTERN_C LPCTSTR NTAPI HrefWebLinkButton(HWND hwndButton)
{
	LPCTSTR pszURL = (LPCTSTR)::GetWindowLong(hwndButton, GWL_USERDATA);
	if (pszURL)
	{
		HWND hwndParent = ::GetParent(hwndButton);
		HWND hwndStatic = ::GetDlgItem(hwndParent, ::GetDlgCtrlID(hwndButton));
		do
		{
			hwndStatic = ::GetWindow(hwndStatic, GW_HWNDNEXT);
		} while (hwndStatic != hwndButton && *(pszURL += lstrlen(pszURL) + 1));
	}
	return pszURL;
}

EXTERN_C LPCTSTR NTAPI OpenWebLinkButton()
{
	HWND hwndButton = ::GetFocus();
	LPCTSTR pszURL = HrefWebLinkButton(hwndButton);
	LONG lStyle = ::GetWindowLong(hwndButton, GWL_STYLE);
	::SetWindowLong(hwndButton, GWL_STYLE, lStyle | BS_LEFTTEXT);
	::RedrawWindow(hwndButton, 0, 0, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
	return pszURL;
}
