/*/ODBCTrace/INI_File.cpp

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
#include "INI_File.h"

UINT CINI_File::GetString(LPCTSTR szSection, LPCTSTR szEntry, LPTSTR szBuffer, UINT cchBuffer) const
{
	TCHAR szDefault[] = TEXT("");
	return GetPrivateProfileString(szSection, szEntry, szDefault, szBuffer, cchBuffer, szFileName);
}

BOOL CINI_File::SetString(LPCTSTR szSection, LPCTSTR szEntry, LPCTSTR szBuffer) const
{
	return WritePrivateProfileString(szSection, szEntry, szBuffer, szFileName);
}

BOOL CINI_File::GetStruct(LPCTSTR szSection, LPCTSTR szEntry, LPVOID pvStruct, UINT cbStruct) const
{
	return GetPrivateProfileStruct(szSection, szEntry, pvStruct, cbStruct, szFileName);
}

BOOL CINI_File::SetStruct(LPCTSTR szSection, LPCTSTR szEntry, LPVOID pvStruct, UINT cbStruct) const
{
	return WritePrivateProfileStruct(szSection, szEntry, pvStruct, cbStruct, szFileName);
}

UINT CINI_File::GetNumber(LPCTSTR szSection, LPCTSTR szEntry, INT nDefault) const
{
	return GetPrivateProfileInt(szSection, szEntry, nDefault, szFileName);
}

BOOL CINI_File::SetNumber(LPCTSTR szSection, LPCTSTR szEntry, UINT nValue) const
{
	TCHAR szBuffer[12];
	wsprintf(szBuffer, TEXT("%u"), nValue);
	return WritePrivateProfileString(szSection, szEntry, szBuffer, szFileName);
}

COLORREF CINI_File::GetColor(LPCTSTR szSection, LPCTSTR szEntry, COLORREF crDefault) const
{
	RGBTRIPLE &rgbt = reinterpret_cast<RGBTRIPLE &>(crDefault);
	TCHAR szBuffer[16];
	if (GetString(szSection, szEntry, szBuffer, 16))
	{
		rgbt.rgbtBlue = (BYTE)PathParseIconLocation(szBuffer);
		rgbt.rgbtGreen = (BYTE)PathParseIconLocation(szBuffer);
		rgbt.rgbtRed = (BYTE)StrToInt(szBuffer);
	}
	return crDefault;
}

BOOL CINI_File::SetColor(LPCTSTR szSection, LPCTSTR szEntry, COLORREF crValue) const
{
	RGBTRIPLE &rgbt = reinterpret_cast<RGBTRIPLE &>(crValue);
	TCHAR szBuffer[16];
	wsprintf(szBuffer, TEXT("%u,%u,%u"), (UINT)rgbt.rgbtRed, (UINT)rgbt.rgbtGreen, (UINT)rgbt.rgbtBlue);
	return WritePrivateProfileString(szSection, szEntry, szBuffer, szFileName);
}

void CINI_File::ScanWindowPlacement(HWND hwnd, LPCTSTR szSection, LPCTSTR szEntry) const
{
	TCHAR szBuffer[60];
	if (GetString(szSection, szEntry, szBuffer, 60))
	{
		WINDOWPLACEMENT wp;
		ZeroMemory(&wp, sizeof wp);
		wp.length = sizeof wp;
		wp.rcNormalPosition.bottom = PathParseIconLocation(szBuffer);
		wp.rcNormalPosition.right = PathParseIconLocation(szBuffer);
		wp.rcNormalPosition.top = PathParseIconLocation(szBuffer);
		wp.rcNormalPosition.left = PathParseIconLocation(szBuffer);
		wp.showCmd = StrToInt(szBuffer);
		::SetWindowPlacement(hwnd, &wp);
	}
}

void CINI_File::DumpWindowPlacement(HWND hwnd, LPCTSTR szSection, LPCTSTR szEntry) const
{
	TCHAR szBuffer[60];
	WINDOWPLACEMENT wp;
	::GetWindowPlacement(hwnd, &wp);
	wsprintf(szBuffer, TEXT("%d,%d,%d,%d,%d"), wp.showCmd, wp.rcNormalPosition);
	SetString(szSection, szEntry, szBuffer);
}

void CINI_File::ScanHeaderItemSizes(HWND hwndHead, LPCTSTR szSection, LPCTSTR szEntry) const
{
	int n = Header_GetItemCount(hwndHead);
	TCHAR szBuffer[1024];
	if (GetString(szSection, szEntry, szBuffer, 1024))
	{
		int i = 0;
		LPCTSTR pszToken = szBuffer;
		if (*pszToken) do
		{
			HDITEM item;
			item.mask = HDI_WIDTH;
			item.cxy = StrToInt(pszToken);
			Header_SetItem(hwndHead, i, &item);
			pszToken = StrChr(pszToken, ',');
		} while (++i < n && pszToken++);
	}
}

void CINI_File::DumpHeaderItemSizes(HWND hwndHead, LPCTSTR szSection, LPCTSTR szEntry) const
{
	int n = Header_GetItemCount(hwndHead);
	TCHAR szBuffer[1024];
	LPTSTR pszToken = szBuffer - 1;
	for (int i = 0 ; i < n ; ++i)
	{
		HDITEM item;
		item.mask = HDI_WIDTH;
		Header_GetItem(hwndHead, i, &item);
		++pszToken;
		pszToken += wsprintf(pszToken, TEXT("%d"), item.cxy);
		*pszToken = ',';
	}
	*pszToken = '\0';
	SetString(szSection, szEntry, szBuffer);
}
