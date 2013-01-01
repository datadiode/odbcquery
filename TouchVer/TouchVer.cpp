/*/TouchVer.cpp

Last edit: 2012-12-31 Jochen Neubeck

[The MIT license]

Copyright (c) 2012 Jochen Neubeck

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

#include <shlwapi.h>
#include "../Common/WinStdIO.h"

#define const // disable const awareness
#include "../Common/VersionData.h"
#undef const // enable const awareness

HANDLE NTAPI FindFile(HANDLE h, LPCTSTR path, WIN32_FIND_DATA *fd)
{
	if (h == INVALID_HANDLE_VALUE)
	{
		h = FindFirstFile(path, fd);
	}
	else if (!FindNextFile(h, fd))
	{
		FindClose(h);
		h = INVALID_HANDLE_VALUE;
	}
	return h;
}

LPVOID NTAPI MapFile(HANDLE h)
{
	LPVOID p = 0;
	DWORD cb = GetFileSize(h, 0);
	if (cb != INVALID_FILE_SIZE)
	{
		h = CreateFileMapping(h, 0, PAGE_READWRITE, 0, cb, 0);
		if (h)
		{
			p = MapViewOfFile(h, FILE_MAP_READ|FILE_MAP_WRITE, 0, 0, cb);
			CloseHandle(h);
		}
	}
	return p;
}

class CDateTimeString
{
public:
	TCHAR date[11];
	TCHAR time[9];
	CDateTimeString(const SYSTEMTIME &st)
	{
		const LCID locale = MAKELCID(MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED), SORT_DEFAULT);
		::GetDateFormat(locale, LOCALE_NOUSEROVERRIDE, &st, 0, date, 11);
		::GetTimeFormat(locale, LOCALE_NOUSEROVERRIDE, &st, 0, time, 9);
	}
	struct va
	{
		LPCTSTR date;
		LPCTSTR time;
		} va() {
		struct va va = { date, time };
		return va;
	}
};

void mainCRTStartup()
{
	LPTSTR lpCmdLine = PathGetArgs(GetCommandLine());
	DWORD dwFileFlagsAdd = 0;
	DWORD dwFileFlagsRemove = 0;
	while (*(lpCmdLine += StrSpn(lpCmdLine, TEXT("\t \r\n"))))
	{
		LPTSTR lpCmdNext = PathGetArgs(lpCmdLine);
		PathRemoveArgs(lpCmdLine);
		switch (*lpCmdLine)
		{
		case '+':
			dwFileFlagsAdd |= StrToInt(lpCmdLine + 1);
			break;
		case '-':
			dwFileFlagsRemove |= StrToInt(lpCmdLine + 1);
			break;
		default:
			LPTSTR lpPattern = PathFindFileName(lpCmdLine);
			WIN32_FIND_DATA fd;
			HANDLE h = INVALID_HANDLE_VALUE;
			while ((h = FindFile(h, lpCmdLine, &fd)) != INVALID_HANDLE_VALUE)
			{
				TCHAR szPath[MAX_PATH];
				*lpPattern = '\0';
				wsprintf(szPath, TEXT("%s%s"), lpCmdLine, fd.cFileName);
				WriteTo<STD_OUTPUT_HANDLE>("%s: ", szPath);
				HANDLE hFile = CreateFile(szPath, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
				if (hFile != INVALID_HANDLE_VALUE)
				{
					if (LPVOID pModule = MapFile(hFile))
					{
						HMODULE hModule = (HMODULE)((UINT_PTR)pModule + 1);
						if (CVersionData *pvd = CVersionData::Load(hModule))
						{
							VS_FIXEDFILEINFO *p = (VS_FIXEDFILEINFO *)pvd->Data();
							FILETIME ft = { p->dwFileDateLS, p->dwFileDateMS };
							SYSTEMTIME st;
							if (FileTimeToSystemTime(&ft, &st))
							{
								WriteTo<STD_OUTPUT_HANDLE>("%s %s -> ", CDateTimeString(st).va());
								GetLocalTime(&st);
								WriteTo<STD_OUTPUT_HANDLE>("%s %s\n", CDateTimeString(st).va());
								SystemTimeToFileTime(&st, &ft);
								p->dwFileFlags = p->dwFileFlags & ~dwFileFlagsRemove | dwFileFlagsAdd;
								p->dwFileDateLS = ft.dwLowDateTime;
								p->dwFileDateMS = ft.dwHighDateTime;
							}
						}
						else
						{
							WriteTo<STD_OUTPUT_HANDLE>("Can't locate version info\n");
						}
					}
					CloseHandle(hFile);
				}
				else
				{
					WriteTo<STD_OUTPUT_HANDLE>("Can't open\n");
				}
			}
		}
		lpCmdLine = lpCmdNext;
	}
	ExitProcess(0);
}
