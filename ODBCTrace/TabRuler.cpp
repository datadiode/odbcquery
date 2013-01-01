/*/ODBCTrace/TabRuler.cpp

Copyright (c) 2006 Jochen Neubeck

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
#include "stdafx.h"

HWND NTAPI TabRuler_Create(HWND hWnd, HWND hwndEdit, int cx, int cy)
{
	HDC hDC = ::GetDC(0);
	int cxy = ::MulDiv(::GetDeviceCaps(hDC, LOGPIXELSY), lDefaultTab, 1440);
	::ReleaseDC(0, hDC);
	RECT rcHead = { 0, 0, cx, cy };
	::MapDialogRect(hWnd, &rcHead);
	HWND hwndFrom = ::CreateWindowEx(0, WC_HEADER, 0, WS_CHILD|WS_VISIBLE, 0, 0, 16320, rcHead.bottom, hWnd, 0, 0, 0);
	if (hwndFrom)
	{
		::SetParent(hwndFrom, hwndEdit);
		for (int i = 0 ; i < MAX_TAB_STOPS ; ++i)
		{
			HDITEM item;
			item.mask = HDI_WIDTH|HDI_FORMAT;
			item.cxy = cxy;
			item.fmt = HDF_STRING;
			Header_InsertItem(hwndFrom, i, &item);
		}
		RECT rcEdit;
		::SendMessage(hwndEdit, EM_GETRECT, 0, (LPARAM)&rcEdit);
		rcEdit.top = rcHead.bottom + 1;
		::SendMessage(hwndEdit, EM_SETRECT, 0, (LPARAM)&rcEdit);
	}
	return hwndFrom;
}

void NTAPI TabRuler_Update(HWND hwndFrom)
{
	HWND hwndEdit = ::GetParent(hwndFrom);
	TEXTMETRIC tm;
	CHARFORMAT cf;
	HDC hDC = ::GetDC(0);
	tm.tmAveCharWidth = (short)::GetDialogBaseUnits();
	cf.cbSize = sizeof cf;
	HFONT hFont = (HFONT)::SendMessage(hwndEdit, WM_GETFONT, 0, 0);
	DWORD dwMask = ::SendMessage(hwndEdit, EM_GETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf);
	if (dwMask)
	{
		LOGFONT lf;
		ZeroMemory(&lf, sizeof lf);
		lf.lfCharSet = DEFAULT_CHARSET;
		lstrcpyn(lf.lfFaceName, cf.szFaceName, LF_FACESIZE);
		RECT rc;
		rc.left = rc.top = rc.right = 0;
		rc.bottom = ::MulDiv(::GetDeviceCaps(hDC, LOGPIXELSY), cf.yHeight, 1440);
		::DPtoLP(hDC, (LPPOINT)&rc, 2);
		lf.lfHeight = rc.bottom - rc.top;
		if (lf.lfHeight > 0)
			lf.lfHeight = -lf.lfHeight;
		hFont = ::CreateFontIndirect(&lf);
	}
	::SelectObject(hDC, hFont);
	::GetTextMetrics(hDC, &tm);
	::ReleaseDC(0, hDC);
	if (dwMask)
	{
		::DeleteObject(hFont);
	}
	INT x = 0;
	INT rgTabStops[MAX_TAB_STOPS];
	int n = Header_GetItemCount(hwndFrom);
	HDITEM item;
	item.mask = HDI_WIDTH;
	for (int i = 0 ; i < n ; ++i)
	{
		Header_GetItem(hwndFrom, i, &item);
		rgTabStops[i] = MulDiv(x += item.cxy, 4, tm.tmAveCharWidth);
	}
	::SendMessage(hwndEdit, EM_SETTABSTOPS, n, LPARAM(rgTabStops));
}

void NTAPI TabRuler_Scroll(HWND hwndFrom)
{
	POINT pt;
	HWND hwndEdit = ::GetParent(hwndFrom);
	::SendMessage(hwndEdit, EM_GETSCROLLPOS, 0, (LPARAM)&pt);
	::SetWindowPos(hwndFrom, 0, -pt.x, 0, 0, 0, SWP_NOSIZE|SWP_NOZORDER|SWP_NOREDRAW|SWP_NOACTIVATE);
	::InvalidateRect(hwndFrom, 0, TRUE);
}
