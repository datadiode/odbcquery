/*/tools.cpp

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

void NTAPI SetDlgCaption(HWND hWnd, LPCTSTR szAppName, LPCTSTR lpIcon)
{
	TCHAR szTitle[2 * MAX_PATH];
	int cchAppName = wsprintf(szTitle, TEXT("%s - "), szAppName);
	::GetWindowText(hWnd, szTitle + cchAppName, MAX_PATH);
	::SetWindowText(hWnd, szTitle);
	if (HINSTANCE hInstance = (HINSTANCE)::GetWindowLong(hWnd, GWL_HINSTANCE))
	{
		if (HICON hIcon = ::LoadIcon(hInstance, lpIcon))
		{
			::SendMessage(hWnd, WM_SETICON, 0, (LPARAM)hIcon);
			::SendMessage(hWnd, WM_SETICON, 1, (LPARAM)hIcon);
		}
	}
}

void NTAPI SetClipboardText(HWND hWnd, LPCTSTR pszText)
{
	if (::OpenClipboard(hWnd))
	{
		DWORD cbText = lstrlen(pszText) + 1;
		HGLOBAL hMem = ::GlobalAlloc(GPTR, cbText);
		lstrcpy(static_cast<LPTSTR>(hMem), pszText);
		::SetClipboardData(CF_TEXT, hMem);
		::CloseClipboard();
	}
}

WORD NTAPI GetModState()
{
	WORD flags = 0;
	if (::GetKeyState(VK_LMENU) < 0)
		flags |= MOD_ALT | MOD_ALT << 4;
	if (::GetKeyState(VK_LCONTROL) < 0)
		flags |= MOD_CONTROL | MOD_CONTROL << 4;
	if (::GetKeyState(VK_LSHIFT) < 0)
		flags |= MOD_SHIFT | MOD_SHIFT << 4;
	if (::GetKeyState(VK_LWIN) < 0)
		flags |= MOD_WIN | MOD_WIN << 4;
	if (::GetKeyState(VK_RMENU) < 0)
		flags |= MOD_ALT | MOD_ALT << 8;
	if (::GetKeyState(VK_RCONTROL) < 0)
		flags |= MOD_CONTROL | MOD_CONTROL << 8;
	if (::GetKeyState(VK_RSHIFT) < 0)
		flags |= MOD_SHIFT | MOD_SHIFT << 8;
	if (::GetKeyState(VK_RWIN) < 0)
		flags |= MOD_WIN | MOD_WIN << 8;
	return flags;
}

#ifdef _DEBUG
void NTAPI AssertionFailed(const char *file, int line)
{
	TCHAR text[512];
	TCHAR path[MAX_PATH];
	GetModuleFileName(0, path, MAX_PATH);
	wsprintf(text, TEXT("%hs(%d) : Assertion failed\n"), file, line);
	OutputDebugString(text);
	switch (MessageBox(0, text, path, MB_TOPMOST | MB_ICONSTOP | MB_TASKMODAL | MB_ABORTRETRYIGNORE))
	{
	case IDABORT:
		TerminateProcess(GetCurrentProcess(), 3);
	case IDRETRY:
		DebugBreak();
	}
}
#endif
