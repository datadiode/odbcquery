/*/RichEditTools.cpp

Copyright (c) 2007 Jochen Neubeck

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

LPTSTR NTAPI GetLineAtChar(HWND hWnd, LONG cp)
{
	TEXTRANGE tr;
	tr.chrg.cpMin = ::SendMessage(hWnd, EM_EXLINEFROMCHAR, 0, cp);
	tr.chrg.cpMin = ::SendMessage(hWnd, EM_LINEINDEX, tr.chrg.cpMin, 0);
	tr.chrg.cpMax = tr.chrg.cpMin + ::SendMessage(hWnd, EM_LINELENGTH, cp, 0);
	tr.lpstrText = (LPTSTR)CoTaskMemAlloc((tr.chrg.cpMax - tr.chrg.cpMin + 1) * sizeof(TCHAR));
	if (tr.lpstrText)
		::SendMessage(hWnd, EM_GETTEXTRANGE, 0, (LPARAM)&tr);
	return tr.lpstrText;
}

HWND NTAPI CreateInfoTip(HWND hWnd)
{
	hWnd = CreateWindow(TEXT("RichEdit20A"), 0, WS_POPUP|ES_MULTILINE|ES_READONLY, 0, 0, 0, 0, hWnd, 0, 0, 0);
	// Non-client borders tend to cause noticeable flicker with bottomless
	// richedit controls, so create a child static black frame instead.
	CreateWindow(TEXT("STATIC"), 0, SS_BLACKFRAME|WS_CHILD|WS_VISIBLE, 0, 0, 0, 0, hWnd, 0, 0, 0);
	SendMessage(hWnd, EM_SETEVENTMASK, 0, ENM_REQUESTRESIZE);
	return hWnd;
}

void NTAPI ResizeInfoTip(REQRESIZE *pParam)
{
	HWND hWnd = pParam->nmhdr.hwndFrom;
	GETTEXTLENGTHEX gtlx = { 0, CP_ACP };
	LONG cpLength = SendMessage(hWnd, EM_GETTEXTLENGTHEX, (WPARAM)&gtlx, 0);
	DWORD xyMin = SendMessage(hWnd, EM_POSFROMCHAR, 0, 0);
	DWORD xyMax = SendMessage(hWnd, EM_POSFROMCHAR, cpLength, 0);
	RECT rc = pParam->rc;
	RECT rcClip;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcClip, FALSE);
	int cxAvail = rcClip.right - rcClip.left;
	int cyAvail = rcClip.bottom - rcClip.top;
	if (GET_Y_LPARAM(xyMin) == GET_Y_LPARAM(xyMax))
	{
		// Guess effective line width
		if (cpLength == 1)
		{
			// GET_X_LPARAM(xyMax) seems a poor guess in this case...
			CHARFORMAT cf;
			cf.cbSize = sizeof cf;
			SendMessage(hWnd, EM_GETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf);
			HDC hDC = GetDC(0);
			int logpixelsX = GetDeviceCaps(hDC, LOGPIXELSY);
			ReleaseDC(0, hDC);
			rc.right = rc.left + MulDiv(cf.yHeight, logpixelsX, 1440);
		}
		else
		{
			rc.right = rc.left + GET_X_LPARAM(xyMax);
		}
	}
	else
	{
		rc.right = rc.left + cxAvail;
	}
	int cx = rc.right - rc.left + 10;
	int cy = rc.bottom - rc.top + 4;
	if (cx > cxAvail)
		cx = cxAvail;
	if (cy > cyAvail)
		cy = cyAvail;
	if (::GetCursorPos((LPPOINT)&rc))
	{
		if (HCURSOR hCursor = ::GetCursor())
		{
			ICONINFO info;
			::GetIconInfo(hCursor, &info);
			rc.top += GetSystemMetrics(SM_CYCURSOR) - 1 - info.yHotspot;
		}
		if (rc.left + cx > cxAvail)
			rc.left = cxAvail - cx;
		if (rc.top + cy > cyAvail)
			rc.top = cyAvail - cy;
		SetWindowPos(hWnd, 0, rc.left, rc.top, cx, cy, SWP_NOZORDER|SWP_NOACTIVATE);
		SetWindowPos(GetTopWindow(hWnd), 0, 0, 0, cx, cy, SWP_NOZORDER|SWP_NOACTIVATE);
		GetClientRect(hWnd, &rc);
		InflateRect(&rc, -4, -2);
		SendMessage(hWnd, EM_SETRECT, 0, (LPARAM)&rc);
	}
}

// Passing wide strings to EM_REPLACESEL is meant to make the functions below
// work with both ANSI and UNICODE.

BOOL NTAPI JoinLines(HWND hwndEdit, CHARRANGE *pcr, BOOL bAllowUndo)
{
	LONG lnMin = ::SendMessage(hwndEdit, EM_EXLINEFROMCHAR, 0, pcr->cpMin);
	LONG lnMax = ::SendMessage(hwndEdit, EM_EXLINEFROMCHAR, 0, pcr->cpMax - 1);
	BOOL bLfAtEnd = lnMax != ::SendMessage(hwndEdit, EM_EXLINEFROMCHAR, 0, pcr->cpMax);
	while (lnMax > lnMin)
	{
		CHARRANGE cr;
		cr.cpMax = ::SendMessage(hwndEdit, EM_LINEINDEX, lnMax, 0);
		cr.cpMin = ::SendMessage(hwndEdit, EM_LINEINDEX, --lnMax, 0);
		cr.cpMin += ::SendMessage(hwndEdit, EM_LINELENGTH, cr.cpMin, 0);
		// Select the line break
		::SendMessage(hwndEdit, EM_EXSETSEL, 0, (LPARAM)&cr);
		// Replace it by a single space - the attempt to do this in a single step fails for me
		::SendMessage(hwndEdit, EM_REPLACESEL, bAllowUndo, (LPARAM)L"");
		::SendMessage(hwndEdit, EM_REPLACESEL, bAllowUndo, (LPARAM)L" ");
		pcr->cpMax -= cr.cpMax - cr.cpMin - 1;
	}
	return bLfAtEnd;
}

void NTAPI JoinSelectedLines(HWND hwndEdit, BOOL bAllowUndo)
{
	CHARRANGE cr;
	::LockWindowUpdate(hwndEdit);
	::SendMessage(hwndEdit, EM_EXGETSEL, 0, (LPARAM)&cr);
	::JoinLines(hwndEdit, &cr, bAllowUndo);
	::SendMessage(hwndEdit, EM_EXSETSEL, 0, (LPARAM)&cr);
	::LockWindowUpdate(0);
	::RedrawWindow(hwndEdit, 0, 0, RDW_ERASE | RDW_INVALIDATE | RDW_UPDATENOW);
}

void NTAPI PasteAsSingleLine(HWND hwndEdit, BOOL bAllowUndo)
{
	CHARRANGE cr;
	::LockWindowUpdate(hwndEdit);
	int nPos = ::GetScrollPos(hwndEdit, SB_VERT);
	::SendMessage(hwndEdit, EM_GETSEL, (LPARAM)&cr.cpMin, 0);
	::SendMessage(hwndEdit, WM_PASTE, 0, 0);
	::SendMessage(hwndEdit, EM_GETSEL, 0, (LPARAM)&cr.cpMax);
	BOOL bLfAtEnd = ::JoinLines(hwndEdit, &cr, bAllowUndo);
	cr.cpMin = cr.cpMax;
	::SendMessage(hwndEdit, EM_EXSETSEL, 0, (LPARAM)&cr);
	if (!bLfAtEnd)
		::SendMessage(hwndEdit, EM_REPLACESEL, bAllowUndo, (LPARAM)L"\n");
	if (nPos != ::GetScrollPos(hwndEdit, SB_VERT))
		if LOWORD(::SendMessage(hwndEdit, EM_SCROLL, SB_LINEUP, 0))
			::SendMessage(hwndEdit, EM_SCROLL, SB_LINEDOWN, 0);
	::SendMessage(hwndEdit, EM_SCROLLCARET, 0, 0);
	::LockWindowUpdate(0);
	::RedrawWindow(hwndEdit, 0, 0, RDW_ERASE | RDW_INVALIDATE | RDW_UPDATENOW);
}
