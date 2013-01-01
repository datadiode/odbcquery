/*/DllProxies.cpp

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
#include "DllProxies.h"

// If MFC is unavailable, resort to FatalAppExit for exceptions.
#ifndef _AFX
#define AfxThrowOleDispatchException FatalAppExit
#endif

// TCHAR-aware function name decoration
#ifdef _UNICODE
#define AW "W"
#else
#define AW "A"
#endif

/**
 * @brief Load a dll and import a number of functions.
 */
LPVOID DllProxy::Load()
{
	if (Names[0])
	{
		if (Names[1] == 0 || Names[1] == Names[0])
			return 0;
		HMODULE handle = LoadLibraryA(Names[0]);
		if (handle == 0)
		{
			Names[1] = 0;
			return 0;
		}
		LPCSTR *p = Names;
		*Names = 0;
		while (LPCSTR name = *++p)
		{
			*p = (LPCSTR)GetProcAddress(handle, name);
			if (*p == 0)
			{
				Names[0] = Names[1] = name;
				Names[2] = (LPCSTR)handle;
				return 0;
			}
		}
		*p = (LPCSTR)handle;
	}
	return this + 1;
}

/**
 * @brief Load a dll and import a number of functions, or throw an exception.
 */
LPVOID DllProxy::EnsureLoad()
{
	if (!Load())
	{
		TCHAR buf[1024];
		int cch = wsprintf(buf, TEXT("%hs"), Names[0]);
		DWORD error = ERROR_MOD_NOT_FOUND;
		if (Names[1])
		{
			buf[cch++] = '@';
			cch += GetModuleFileName((HMODULE)Names[2], buf + cch, MAX_PATH);
			error = ERROR_PROC_NOT_FOUND;
		}
		buf[cch++] = ':';
		buf[cch++] = '\n';
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, error, 0, buf + cch, MAX_PATH, 0);
		AfxThrowOleDispatchException(0, buf);
	}
	return this + 1;
}

/**
 * @brief PSAPI dll proxy
 */
DllProxy::Instance<struct PSAPI> PSAPI =
{
	"PSAPI",
	"EnumProcesses",
	"EnumProcessModules",
	"GetModuleFileNameEx"AW,
	static_cast<HMODULE>(0)
};

/**
 * @brief TOOLHELP dll proxy
 */
DllProxy::Instance<struct TOOLHELP> TOOLHELP =
{
	"KERNEL32",
	"CreateToolhelp32Snapshot",
	"Module32First",
	"Module32Next",
	static_cast<HMODULE>(0)
};

/**
 * @brief SHELL32NT6 dll proxy
 */
DllProxy::Instance<struct SHELL32NT6> SHELL32NT6 =
{
	"SHELL32",
	"SHGetKnownFolderPath",
	static_cast<HMODULE>(0)
};
