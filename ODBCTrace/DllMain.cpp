/*/ODBCTrace/DllMain.cpp

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
#include "resource.h"
#include "TraceWnd.h"
#include "tools.h"

struct ODBCTraceState ODBCTrace;

struct ODBCTraceState *GetODBCTrace(DWORD dwVersion)
{
	if (LOWORD(dwVersion) > LOWORD(ODBCTraceStateVersion))
		return 0;
	if (HIWORD(dwVersion) > HIWORD(ODBCTraceStateVersion))
		return 0;
	return (struct ODBCTraceState *)((LPBYTE)&ODBCTrace.dwVersion - LOWORD(dwVersion));
}

struct BootstrapRecord
{
	TCHAR szAppPath[MAX_PATH];
	struct ODBCTraceState ODBCTrace;
	BOOL bDone;
};

LPVOID NTAPI ShareAlloc(LPCTSTR lpName, DWORD dwSize)
{
	LPVOID pMem = 0;
	if (HANDLE hMem = CreateFileMapping(INVALID_HANDLE_VALUE, 0, PAGE_READWRITE, 0, dwSize, lpName))
	{
		if (dwSize)
		{
			pMem = MapViewOfFile(hMem, FILE_MAP_ALL_ACCESS, 0, 0, dwSize);
		}
		else
		{
			CloseHandle(hMem);
		}
	}
	return pMem;
}

BOOL CALLBACK ODBCTraceBootstrap(HWND hwnd, HINSTANCE hinst, LPTSTR lpCmdLine, int nCmdShow)
{
	// In-process invocation through client app?
	if (lpCmdLine == 0)
	{
		if (::DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_OPTIONS), hwnd, COptionsDlg::DlgProc) == IDOK)
		{
			if (nCmdShow)
			{
				CTraceWnd::InitializeClass();
			}
			return TRUE;
		}
		return FALSE;
	}
	// Out-of-process invocation
	if (BootstrapRecord *p = (BootstrapRecord *)ShareAlloc(lpCmdLine, sizeof BootstrapRecord))
	{
		HWND hwndForeground = ::GetForegroundWindow();
		lstrcpy(g_szAppPath, p->szAppPath);
		g_szAppName = ::PathFindFileName(g_szAppPath);
		if (::DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_OPTIONS), 0, COptionsDlg::DlgProc) == IDOK)
		{
			p->ODBCTrace = ODBCTrace;
			p->bDone = TRUE;
		}
		UnmapViewOfFile(p);
		ShareAlloc(lpCmdLine, 0);
		if (hwndForeground)
			::SetForegroundWindow(hwndForeground);
	}
	else
	{
		MessageBox(hwnd, GetCommandLine(), 0, MB_ICONSTOP);
	}
	return FALSE;
}

BOOL NTAPI RunBootstrap()
{
	//return ::DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_OPTIONS), 0, COptionsDlg::DlgProc) == IDOK;
	BOOL bDone = FALSE;
	LPTSTR lpCmdLine = GetCommandLine();
	TCHAR szCmd[800];
	TCHAR szInst[MAX_PATH];
	GetModuleFileName(g_hInstance, szInst, MAX_PATH);
	GetShortPathName(szInst, szInst, MAX_PATH);
	int cchCmd = wsprintf
	(
		szCmd,
		"RUNDLL32.EXE %s,ODBCTraceBootstrap ODBCTrace.BootstrapRecord+0x%08lx",
		szInst, GetCurrentProcessId()
	) + 1 - sizeof "ODBCTrace.BootstrapRecord+0x12345678";
	if (lstrlen(lpCmdLine) < cchCmd ||
		CompareString(LOCALE_SYSTEM_DEFAULT, 0, lpCmdLine, cchCmd, szCmd, cchCmd) != CSTR_EQUAL)
	{
		lpCmdLine = szCmd + cchCmd;
		if (BootstrapRecord *p = (BootstrapRecord *)ShareAlloc(lpCmdLine, sizeof BootstrapRecord))
		{
			struct
			{
				STARTUPINFO si;
				PROCESS_INFORMATION pi;
			} r;
			ZeroMemory(&r, sizeof r);
			lstrcpy(p->szAppPath, g_szAppPath);
			if (CreateProcess(0, szCmd, 0, 0, FALSE, 0, 0, 0, &r.si, &r.pi)) 
			{
				WaitForSingleObject(r.pi.hProcess, INFINITE);
				CloseHandle(r.pi.hProcess);
				CloseHandle(r.pi.hThread);
				if (p->bDone)
				{
					ODBCTrace = p->ODBCTrace;
					bDone = TRUE;
				}
			}
			UnmapViewOfFile(p);
			ShareAlloc(lpCmdLine, 0);
		}
	}
	return bDone;
}

BOOL APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID pvReserved)
{
	switch (dwReason) 
	{
	case DLL_PROCESS_ATTACH:
		g_hInstance = hInstance;
		::GetProfilePath(g_szAppPath, TRUE);
		::PathAppend(g_szAppPath, TEXT("ODBCTrace.ini"));
		pAppProfile = CINI_File::From(StrDup(g_szAppPath));
		::GetModuleFileName(0, g_szAppPath, MAX_PATH);
		g_szAppName = ::PathFindFileName(g_szAppPath);
		ZeroMemory(&ODBCTrace, sizeof ODBCTrace);
		ODBCTrace.dwVersion = ODBCTraceStateVersion - ODBCTrace.cbUnused;
		ODBCTrace.fSendToWindow = TRUE;
		ODBCTrace.cpHighWaterMark = 0x10000;
		ODBCTrace.nLimitRowDump = 3;
		CPerformanceCounterString::InitializeClass();
		CallStack::InitializeClass();
		if (pvReserved == 0 && ::GetModuleHandle("ODBCAD32.EXE") == 0 && ::RunBootstrap())
		{
			CTraceWnd::InitializeClass();
		}
		if ((stack.dwTlsIndex = TlsAlloc()) == 0xFFFFFFFF) 
			return FALSE; 
		// No break: Initialize the index for first thread.
	case DLL_THREAD_ATTACH:
		if (GetCurrentThreadId() != CTraceWnd::dwThreadId)
			TlsSetValue(stack.dwTlsIndex, new CallStack());
		break;
	case DLL_THREAD_DETACH:
		if (GetCurrentThreadId() != CTraceWnd::dwThreadId)
			delete stack.operator->();
		break;
	case DLL_PROCESS_DETACH:
		ODBCTrace.hWnd = 0;
		delete stack.operator->();
		TlsFree(stack.dwTlsIndex);
		break;
	}
	return TRUE;
}
