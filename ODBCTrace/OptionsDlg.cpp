/*/ODBCTrace/OptionsDlg.cpp

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

#define RTL_RANGE_SIZE(type, from, to) RTL_SIZEOF_THROUGH_FIELD(type, to) - FIELD_OFFSET(type, from)

enum
{
	cfHead = 10,
	cfBody = RTL_RANGE_SIZE(struct ODBCTraceState, SQLAllocConnect, SQLCopyDesc),
	cfTraceFilter = cfHead + cfBody
};

void COptionsDlg::PopulateLbFilter(HWND hWnd)
{
	TCHAR rgfProfile[cfTraceFilter + 2];
	LPBYTE pfName = &ODBCTrace.SQLAllocConnect;
	const char *pchLower = ODBCTraceState;
	while (LPTSTR pchUpper = StrChrA(pchLower, ';'))
	{
		pchLower = StrRChrA(pchLower, pchUpper, VK_SPACE);
		char szName[64];
		lstrcpynA(szName, pchLower + 1, pchUpper - pchLower);
		int i = ListBox_AddString(hLbFilter, szName);
		ListBox_SetItemData(hLbFilter, i, pfName);
		ListBox_SetSel(hLbFilter, *pfName, i);
		++pfName;
		pchLower = pchUpper + 1;
	}
	int cchProfile = pAppProfile->GetString(TEXT("Filter"), g_szAppName, rgfProfile, RTL_NUMBER_OF(rgfProfile));
	if (cchProfile == cfTraceFilter)
	{
		CheckDlgButton(hWnd, IDC_TG_SEND_TO_WINDOW, rgfProfile[0] - '0');
		CheckDlgButton(hWnd, IDC_TG_SEND_TO_FILE, rgfProfile[1] - '0');
		BOOLEAN fTopMost = rgfProfile[2] - '0';
		if (ODBCTrace.hWnd && fTopMost != ODBCTrace.fTopMost)
		{
			SendDlgItemMessage(hWnd, IDC_TG_TOPMOST, BM_SETSTYLE, BS_AUTO3STATE, 0);
			fTopMost = 2;
		}
		CheckDlgButton(hWnd, IDC_TG_TOPMOST, fTopMost);
		CheckDlgButton(hWnd, IDC_TG_SERIALIZE_CALLS, rgfProfile[3] - '0');
		CheckDlgButton(hWnd, IDC_TG_HIDE_NESTED_CALLS, rgfProfile[4] - '0');
		CheckDlgButton(hWnd, IDC_TG_HIDE_DIAG_INFO, rgfProfile[5] - '0');
		BOOLEAN fDisableTrace = rgfProfile[6] - '0';
		if (ODBCTrace.hWnd && fDisableTrace != ODBCTrace.fDisableTrace)
		{
			SendDlgItemMessage(hWnd, IDC_TG_DISABLE_TRACE, BM_SETSTYLE, BS_AUTO3STATE, 0);
			fDisableTrace = 2;
		}
		CheckDlgButton(hWnd, IDC_TG_DISABLE_TRACE, fDisableTrace);
		CheckDlgButton(hWnd, IDC_TG_WAIT_FOR_CLOSE, rgfProfile[7] - '0');
		CheckDlgButton(hWnd, IDC_TG_CATCH_DEBUG_STRINGS, rgfProfile[8] - '0');
		CheckDlgButton(hWnd, IDC_TG_TRACE_THREAD_LIFETIME, rgfProfile[9] - '0');
		for (int i = 0, n = ListBox_GetCount(hLbFilter) ; i < n ; ++i)
		{
			ListBox_SetSel(hLbFilter, rgfProfile[cfHead + i] == '1', i);
		}
	}
	else
	{
		CheckDlgButton(hWnd, IDC_TG_SEND_TO_WINDOW, 1);
		CheckDlgButton(hWnd, IDC_TG_TOPMOST, ODBCTrace.fTopMost);
		CheckDlgButton(hWnd, IDC_TG_DISABLE_TRACE, ODBCTrace.fDisableTrace);
	}
}

void COptionsDlg::ApplyLbFilter(HWND hWnd)
{
	TCHAR rgfProfile[cfTraceFilter + 2];
	ODBCTrace.fSendToWindow = (BOOLEAN)IsDlgButtonChecked(hWnd, IDC_TG_SEND_TO_WINDOW);
	ODBCTrace.fSendToFile = (BOOLEAN)IsDlgButtonChecked(hWnd, IDC_TG_SEND_TO_FILE);
	BOOLEAN fTopMost = (BOOLEAN)IsDlgButtonChecked(hWnd, IDC_TG_TOPMOST);
	if (fTopMost == 2)
	{
		fTopMost = !ODBCTrace.fTopMost;
	}
	else
	{
		ODBCTrace.fTopMost = fTopMost;
	}
	ODBCTrace.fSerialize = (BOOLEAN)IsDlgButtonChecked(hWnd, IDC_TG_SERIALIZE_CALLS);
	ODBCTrace.fHideNestedCalls = (BOOLEAN)IsDlgButtonChecked(hWnd, IDC_TG_HIDE_NESTED_CALLS);
	ODBCTrace.fHideDiagInfo = (BOOLEAN)IsDlgButtonChecked(hWnd, IDC_TG_HIDE_DIAG_INFO);
	BOOLEAN fDisableTrace = (BOOLEAN)IsDlgButtonChecked(hWnd, IDC_TG_DISABLE_TRACE);
	if (fDisableTrace == 2)
	{
		fDisableTrace = !ODBCTrace.fDisableTrace;
	}
	else
	{
		ODBCTrace.fDisableTrace = fDisableTrace;
	}
	ODBCTrace.fWaitForClose = (BOOLEAN)IsDlgButtonChecked(hWnd, IDC_TG_WAIT_FOR_CLOSE);
	ODBCTrace.fCatchDebugStrings = (BOOLEAN)IsDlgButtonChecked(hWnd, IDC_TG_CATCH_DEBUG_STRINGS);
	ODBCTrace.fTraceThreadLifetime = (BOOLEAN)IsDlgButtonChecked(hWnd, IDC_TG_TRACE_THREAD_LIFETIME);
	rgfProfile[0] = '0' + ODBCTrace.fSendToWindow;
	rgfProfile[1] = '0' + ODBCTrace.fSendToFile;
	rgfProfile[2] = '0' + fTopMost;
	rgfProfile[3] = '0' + ODBCTrace.fSerialize;
	rgfProfile[4] = '0' + ODBCTrace.fHideNestedCalls;
	rgfProfile[5] = '0' + ODBCTrace.fHideDiagInfo;
	rgfProfile[6] = '0' + fDisableTrace;
	rgfProfile[7] = '0' + ODBCTrace.fWaitForClose;
	rgfProfile[8] = '0' + ODBCTrace.fCatchDebugStrings;
	rgfProfile[9] = '0' + ODBCTrace.fTraceThreadLifetime;
	for (int i = 0, n = ListBox_GetCount(hLbFilter) ; i < n ; ++i)
	{
		BYTE fName = ListBox_GetSel(hLbFilter, i) != 0;
		if (LPBYTE pfName = (LPBYTE)ListBox_GetItemData(hLbFilter, i))
		{
			*pfName = fName;
		}
		rgfProfile[cfHead + i] = '0' + fName;
	}
	rgfProfile[cfTraceFilter] = '\0';
	pAppProfile->SetString(TEXT("Filter"), g_szAppName, rgfProfile);
}

COLORREF COptionsDlg::OnPbColor(HWND hwnd, COLORREF rgb, LPCTSTR lpEntry)
{
	struct cc : CHOOSECOLOR
	{
		COLORREF rgCustColors[16];
	} cc;
	ZeroMemory(&cc, sizeof(cc));
	cc.lStructSize = sizeof(CHOOSECOLOR);
	cc.hwndOwner = hwnd;
	cc.lpCustColors = cc.rgCustColors;
	cc.rgbResult = rgb;
	cc.Flags = CC_FULLOPEN | CC_RGBINIT;
	pAppProfile->GetStruct(TEXT("OptionsDlg"), TEXT("rgCustColors"), cc.rgCustColors, sizeof cc.rgCustColors);
	if (::ChooseColor(&cc))
	{
		pAppProfile->SetColor(TEXT("TraceWnd"), lpEntry, rgb = cc.rgbResult);
		pAppProfile->SetStruct(TEXT("OptionsDlg"), TEXT("rgCustColors"), cc.rgCustColors, sizeof cc.rgCustColors);
	}
	return rgb;
}

BOOL COptionsDlg::OnTgSendToFile(HWND hwnd)
{
	TCHAR caption[1024];
	struct ofn : OPENFILENAME
	{
		TCHAR szFile[MAX_PATH];
	} ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFile = ofn.szFile;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrDefExt = CTraceWnd::szFileTitle + sizeof "ODBCTrace";
	ofn.lpstrTitle = caption;
	if (!pAppProfile->GetString(TEXT("LogFile"), g_szAppName, ofn.szFile, MAX_PATH))
	{
		::GetProfilePath(ofn.szFile, TRUE);
		::PathAppend(ofn.szFile, CTraceWnd::szFileTitle);
	}
	wsprintf(caption, TEXT("%s - ODBCTrace Output File"), g_szAppName);
	BOOL bDone = ::GetSaveFileName(&ofn);
	if (bDone)
	{
		pAppProfile->SetString(TEXT("LogFile"), g_szAppName, ofn.szFile);
	}
	return bDone;
}

static BOOL NTAPI KillClientProcess()
{
	HANDLE hProcess = 0;
	if (ODBCTrace.hWnd)
	{
		hProcess = ::GetCurrentProcess();
	}
	else if (LPTSTR lpProcess = StrRChr(GetCommandLine(), 0, '+'))
	{
		int idProcess = 0;
		if (StrToIntEx(lpProcess, STIF_SUPPORT_HEX, &idProcess))
			hProcess = ::OpenProcess(PROCESS_TERMINATE, FALSE, idProcess);
	}
	return hProcess && ::TerminateProcess(hProcess, IDABORT) && ::CloseHandle(hProcess);
}

HWND COptionsDlg::hLbFilter = 0;

BOOL CALLBACK COptionsDlg::DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static CFloatState *FloatState = 0;
	static HWND hCbHighWaterMark = 0;
	static HWND hCbHighWaterMarkFile = 0;
	static const LONG FloatScript[] =
	{
		IDC_FS_FILTER,					BY<1000>::X2R | BY<1000>::Y2B,
		IDC_LB_FILTER,					BY<1000>::X2R | BY<1000>::Y2B,
		IDC_FS_SEND_TO,					BY<1000>::X2L | BY<1000>::X2R,
		IDC_TG_SEND_TO_WINDOW,			BY<1000>::X2L | BY<1000>::X2R,
		IDC_TG_SEND_TO_FILE,			BY<1000>::X2L | BY<1000>::X2R,
		IDC_TG_SERIALIZE_CALLS,			BY<1000>::X2L | BY<1000>::X2R,
		IDC_TG_HIDE_NESTED_CALLS,		BY<1000>::X2L | BY<1000>::X2R,
		IDC_TG_HIDE_DIAG_INFO,			BY<1000>::X2L | BY<1000>::X2R,
		IDC_TG_DISABLE_TRACE,			BY<1000>::X2L | BY<1000>::X2R,
		IDC_TG_WAIT_FOR_CLOSE,			BY<1000>::X2L | BY<1000>::X2R,
		IDC_TG_CATCH_DEBUG_STRINGS,		BY<1000>::X2L | BY<1000>::X2R,
		IDC_TG_TRACE_THREAD_LIFETIME,	BY<1000>::X2L | BY<1000>::X2R,
		IDC_FS_WINDOW,					BY<1000>::X2L | BY<1000>::X2R,
		IDC_TG_TOPMOST,					BY<1000>::X2L | BY<1000>::X2R,
		IDC_PB_BKGND_COLOR,				BY<1000>::X2L | BY<1000>::X2R,
		IDC_PB_POPUP_COLOR,				BY<1000>::X2L | BY<1000>::X2R,
		IDC_FS_EMERGENCY,				BY<1000>::X2L | BY<1000>::X2R,
		IDC_PB_KILL_PROCESS,			BY<1000>::X2L | BY<1000>::X2R,
		IDC_CB_HIGHWATERMARK|L2L|L2R,	BY<1000>::X2L | BY<1000>::X2R,
		IDC_FS_FILE,					BY<1000>::X2L | BY<1000>::X2R,
		IDC_CB_HWM_FILE|L2L|L2R,		BY<1000>::X2L | BY<1000>::X2R,
		IDC_ED_ROWDUMPLIMIT|L2L|L2R,	BY<1000>::X2L | BY<1000>::X2R,
		IDC_PB_SELECT_ALL,				BY<1000>::X2L | BY<1000>::X2R | BY<1000>::Y2T | BY<1000>::Y2B,
		IDC_PB_UNSELECT_ALL,			BY<1000>::X2L | BY<1000>::X2R | BY<1000>::Y2T | BY<1000>::Y2B,
		IDOK,							BY<1000>::X2L | BY<1000>::X2R | BY<1000>::Y2T | BY<1000>::Y2B,
		IDCANCEL,						BY<1000>::X2L | BY<1000>::X2R | BY<1000>::Y2T | BY<1000>::Y2B,
		0
	};
	LONG cpHighWaterMark;
	switch (uMsg)
	{
	case WM_INITDIALOG:
		SetDlgCaption(hWnd, g_szAppName, MAKEINTRESOURCE(IDD_OPTIONS));
		FloatState = new CFloatState(FloatScript);
		FloatState->Clear();
		hLbFilter = ::GetDlgItem(hWnd, IDC_LB_FILTER);
		hCbHighWaterMark = ::GetDlgItem(hWnd, IDC_CB_HIGHWATERMARK);
		hCbHighWaterMarkFile = ::GetDlgItem(hWnd, IDC_CB_HWM_FILE);
		ODBCTrace.cpHighWaterMark = pAppProfile->GetNumber(TEXT("TraceWnd"), TEXT("cpHighWaterMark"), 64 * 1024);
		ODBCTrace.cpHighWaterMarkFile = pAppProfile->GetNumber(TEXT("TraceWnd"), TEXT("cpHighWaterMarkFile"), 16 * 1024 * 1024);
		for (cpHighWaterMark = 16 * 1024 ; cpHighWaterMark <= 256 * 1024 * 1024 ; cpHighWaterMark <<= 2)
		{
			TCHAR szText[40];
			StrFormatByteSize(cpHighWaterMark, szText, RTL_NUMBER_OF(szText));
			int i = ComboBox_AddString(hCbHighWaterMark, szText);
			if (cpHighWaterMark == ODBCTrace.cpHighWaterMark)
				ComboBox_SetCurSel(hCbHighWaterMark, i);
			i = ComboBox_AddString(hCbHighWaterMarkFile, szText);
			if (cpHighWaterMark == ODBCTrace.cpHighWaterMarkFile)
				ComboBox_SetCurSel(hCbHighWaterMarkFile, i);
		}
		ODBCTrace.nLimitRowDump = pAppProfile->GetNumber(TEXT("TraceWnd"), TEXT("nLimitRowDump"), 3);
		::SendDlgItemMessage(hWnd, IDC_ED_ROWDUMPLIMIT, EM_LIMITTEXT, 5, 0);
		::SetDlgItemInt(hWnd, IDC_ED_ROWDUMPLIMIT, ODBCTrace.nLimitRowDump, FALSE);
		PopulateLbFilter(hWnd);
		ListBox_SetCaretIndex(hLbFilter, 0);
		return TRUE;
	case WM_NCDESTROY:
		delete FloatState;
		break;
	case WM_WINDOWPOSCHANGING:
		FloatState->Float(reinterpret_cast<WINDOWPOS *>(lParam));
		break;
	case WM_COMMAND:
		switch (wParam)
		{
		case IDC_PB_SELECT_ALL:
			ListBox_SetSel(hLbFilter, TRUE, -1);
			return TRUE;
		case IDC_PB_UNSELECT_ALL:
			ListBox_SetSel(hLbFilter, FALSE, -1);
			return TRUE;
		case IDC_PB_BKGND_COLOR:
			CTraceWnd::rgbBkgnd = OnPbColor(hWnd, CTraceWnd::rgbBkgnd, TEXT("rgbBkgnd"));
			return TRUE;
		case IDC_PB_POPUP_COLOR:
			CTraceWnd::rgbPopup = OnPbColor(hWnd, CTraceWnd::rgbPopup, TEXT("rgbPopup"));
			return TRUE;
		case IDC_PB_KILL_PROCESS:
			if (KillClientProcess())
				EndDialog(hWnd, IDABORT);
			else
				MessageBox(hWnd, "Unable to kill client process", 0, MB_ICONSTOP);
			return TRUE;
		case IDC_TG_SEND_TO_FILE:
			if (IsDlgButtonChecked(hWnd, IDC_TG_SEND_TO_FILE))
			{
				CheckDlgButton(hWnd, IDC_TG_SEND_TO_FILE, OnTgSendToFile(hWnd));
			}
			else if (ODBCTrace.pStream)
			{
				ODBCTrace.pStream->Release();
				ODBCTrace.pStream = 0;
			}
			return TRUE;
		case MAKEWPARAM(IDC_LB_FILTER, LBN_DBLCLK):
			wParam = IDOK;
			// fall through
		case IDOK:
			ApplyLbFilter(hWnd);
			ODBCTrace.cpHighWaterMark = 16 * 1024 << 2 * ComboBox_GetCurSel(hCbHighWaterMark);
			pAppProfile->SetNumber(TEXT("TraceWnd"), TEXT("cpHighWaterMark"), ODBCTrace.cpHighWaterMark);
			ODBCTrace.cpHighWaterMarkFile = 16 * 1024 << 2 * ComboBox_GetCurSel(hCbHighWaterMarkFile);
			pAppProfile->SetNumber(TEXT("TraceWnd"), TEXT("cpHighWaterMarkFile"), ODBCTrace.cpHighWaterMarkFile);
			ODBCTrace.nLimitRowDump = ::GetDlgItemInt(hWnd, IDC_ED_ROWDUMPLIMIT, 0, FALSE);
			pAppProfile->SetNumber(TEXT("TraceWnd"), TEXT("nLimitRowDump"), ODBCTrace.nLimitRowDump);
			// fall through
		case IDCANCEL:
			EndDialog(hWnd, wParam);
			return TRUE;
		}
		break;
	case WM_GETMINMAXINFO:
		CFloatState::AdjustMax(hWnd, reinterpret_cast<MINMAXINFO *>(lParam));
		break;
	}
	return 0;
}
