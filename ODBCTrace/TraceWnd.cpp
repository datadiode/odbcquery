/*/ODBCTrace/TraceWnd.cpp

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
#include <msdaguid.h>
#include "resource.h"
#include "tools.h"
#include "DllProxies.h"
#include "..\Common\ModuleDirectory.h"
#include "TabRuler.h"
#include "RichEditTools.h"
#include "FormatString.h"
#include "Tokenizer.h"
#include "TraceWnd.h"

HINSTANCE g_hInstance = 0;
TCHAR g_szAppPath[MAX_PATH], *g_szAppName;

const CINI_File *pAppProfile = NULL;

STDAPI GetProfilePath(LPTSTR path, BOOL fCreate)
{
	HRESULT hr = S_OK;
	if (SHELL32NT6::DLL SHELL32 = SHELL32NT6)
	{
		LPWSTR wszPath = NULL;
		DWORD dwFlags = fCreate ? KF_FLAG_CREATE : 0;
		hr = SHELL32->SHGetKnownFolderPath(FOLDERID_LocalAppDataLow, dwFlags, NULL, &wszPath);
		if (SUCCEEDED(hr))
		{
			int len = wnsprintf(path, MAX_PATH, TEXT("%ls"), wszPath);
			if (wszPath[len] != L'\0')
				hr = HRESULT_FROM_WIN32(ERROR_BUFFER_OVERFLOW);
			CoTaskMemFree(wszPath);
		}
	}
	else if (!SHGetSpecialFolderPath(NULL, path, CSIDL_LOCAL_APPDATA, fCreate))
	{
		DWORD err = GetLastError();
		hr = HRESULT_FROM_WIN32(err);
	}
	if (SUCCEEDED(hr))
	{
		PathAppend(path, TEXT("ODBCTrace"));
		if (fCreate)
			CreateDirectory(path, NULL);
	}
	return hr;
}

static int CALLBACK PopulateCbFontFace(const LOGFONT *lplf, const TEXTMETRIC *lptm, DWORD dwType, LPARAM lpData)
{
	ComboBox_AddString((HWND)lpData, lplf->lfFaceName);
	return TRUE;
}

const TCHAR CTraceWnd::szFileTitle[] = TEXT("ODBCTrace.LOG");

COLORREF CTraceWnd::rgbEnter = RGB(0,0,255);
COLORREF CTraceWnd::rgbLeave = RGB(0,128,0);
COLORREF CTraceWnd::rgbError = RGB(255,0,0);
COLORREF CTraceWnd::rgbParam = RGB(0,0,0);
COLORREF CTraceWnd::rgbSqlEx = RGB(128,128,128);
COLORREF CTraceWnd::rgbDebug = RGB(255,128,0);
COLORREF CTraceWnd::rgbTlftm = RGB(255,0,128);
COLORREF CTraceWnd::rgbBkgnd = RGB(255,255,192);
COLORREF CTraceWnd::rgbPopup = RGB(128,255,255);

HANDLE CTraceWnd::hEventInitComplete = 0;

UINT CTraceWnd::commdlg_FindReplace = 0;

HWND CTraceWnd::hwndMain = 0;
HWND CTraceWnd::hwndEdit = 0;
HWND CTraceWnd::hwndHead = 0;
HWND CTraceWnd::hwndFind = 0;
HWND CTraceWnd::hwndTip = 0;
DWORD CTraceWnd::dwThreadId = 0;
HANDLE CTraceWnd::hThread = 0;

struct CTraceWnd::fr CTraceWnd::fr;

void CTraceWnd::InitializeClass()
{
	commdlg_FindReplace = ::RegisterWindowMessage(FINDMSGSTRING);
	rgbBkgnd = pAppProfile->GetColor(TEXT("TraceWnd"), TEXT("rgbBkgnd"), rgbBkgnd);
	rgbPopup = pAppProfile->GetColor(TEXT("TraceWnd"), TEXT("rgbPopup"), rgbPopup);
	hEventInitComplete = ::CreateEvent(0, TRUE, FALSE, 0);
	hThread = ::CreateThread(0, 0, ThreadProc, 0, 0, &dwThreadId);
}

BOOL CALLBACK CTraceWnd::DlgProcAbout(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static const TCHAR szLicense[]
	(
		TEXT("Distributed under the GNU Lesser General Public License.\n")
		TEXT("See http://www.gnu.org/licenses/lgpl.html for details.\n")
		TEXT("\n")
		TEXT("Read Doga Arinir's article on ODBC tracing at\n")
		TEXT("http://www.codeproject.com/database/ODBCTracer.asp.\n")
		TEXT("\n")
		TEXT("Icons taken from http://www.schworak.com.\n")
		TEXT("\n")
		TEXT("Project Home: http://odbctrace.sourceforge.net\n")
	);
	HWND hwndEdit;
	TCHAR szVersion[40];
	LONG lStyle;
	switch (uMsg)
	{
	case WM_INITDIALOG:
		if (HRSRC hRes = ::FindResource(g_hInstance, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION))
		{
			struct VersionData
			{ 
				WORD   wLength; 
				WORD   wValueLength; 
				WORD   wType;
				WCHAR  szKey[sizeof"VS_VERSION_INFO"];
				VS_FIXEDFILEINFO FixedFileInfo;
			};
			C_ASSERT(sizeof(struct VersionData) % sizeof(int) == 0);
			if (struct VersionData *pRes = (struct VersionData *)::LoadResource(g_hInstance, hRes))
			{
				int cch = wsprintf(szVersion, TEXT("ODBCTrace %u.%02u"), UINT HIWORD(pRes->FixedFileInfo.dwFileVersionMS), UINT LOWORD(pRes->FixedFileInfo.dwFileVersionMS));
				if (pRes->FixedFileInfo.dwFileFlags & VS_FF_PRERELEASE)
					szVersion[cch++] = 'b';
				if (pRes->FixedFileInfo.dwFileFlags & VS_FF_DEBUG)
					szVersion[cch++] = 'D';
				szVersion[cch] = '\0';
				::SetDlgItemText(hWnd, IDC_ST_VERSION, szVersion);
				if (pRes->FixedFileInfo.dwFileDateMS)
				{
					FILETIME ft = { pRes->FixedFileInfo.dwFileDateLS, pRes->FixedFileInfo.dwFileDateMS };
					SYSTEMTIME st;
					FileTimeToSystemTime(&ft, &st);
					::GetDateFormat(LOCALE_SYSTEM_DEFAULT, 0, &st, TEXT("yyyy-MM-dd"), szVersion, sizeof "yyyy-MM-dd");
					::SetDlgItemText(hWnd, IDC_ST_VERDATE, szVersion);
				}
			}
		}
		wsprintf(szVersion, TEXT("API Level %u.%u"), UINT HIWORD(ODBCTrace.dwVersion), UINT LOWORD(ODBCTrace.dwVersion));
		::SetDlgItemText(hWnd, IDC_ST_API_LEVEL, szVersion);
		hwndEdit = ::GetDlgItem(hWnd, IDC_ED_LINKS);
		::SendMessage(hwndEdit, EM_SETEVENTMASK, 0, ENM_LINK | ENM_REQUESTRESIZE);
		::SendMessage(hwndEdit, EM_SETBKGNDCOLOR, 0, ::GetSysColor(COLOR_BTNFACE));
		::SendMessage(hwndEdit, EM_SETOPTIONS, ECOOP_OR, ECO_NOHIDESEL);
		::SendMessage(hwndEdit, EM_AUTOURLDETECT, TRUE, 0);
		::SetWindowText(hwndEdit, szLicense);
		lStyle = ::GetWindowLong(hwndEdit, GWL_EXSTYLE);
		::SetWindowLong(hwndEdit, GWL_EXSTYLE, lStyle & ~WS_EX_NOPARENTNOTIFY);
		return TRUE;
	case WM_PARENTNOTIFY:
		if (LOWORD(wParam) == WM_LBUTTONDOWN)
		{
			POINT pt = { SHORT LOWORD(lParam), SHORT HIWORD(lParam) };
			if (HWND hwndHit = ::ChildWindowFromPoint(hWnd, pt))
			{
				::SetFocus(hwndHit);
			}
		}
		break;
	case WM_NOTIFY:
		if (UNotify *pParam = reinterpret_cast<UNotify *>(lParam))
		{
			switch (pParam->NMHDR.code)
			{
			case EN_LINK:
				{
					TEXTRANGE txrg;
					txrg.chrg = pParam->ENLINK.chrg;
					SIZE_T cchText = txrg.chrg.cpMax - txrg.chrg.cpMin;
					switch (pParam->ENLINK.msg)
					{
					case WM_LBUTTONDOWN:
						::SendMessage(pParam->hwnd, EM_EXSETSEL, 0, (LPARAM)&pParam->ENLINK.chrg);
						break;
					case WM_LBUTTONUP:
					case WM_LBUTTONDBLCLK:
						if (cchText < MAX_PATH)
						{
							TCHAR szURL[MAX_PATH];
							txrg.lpstrText = szURL;
							::SendMessage(pParam->hwnd, EM_GETTEXTRANGE, 0, (LPARAM)&txrg);
							::ShellExecute(0, TEXT("open"), txrg.lpstrText, 0, 0, SW_SHOWNORMAL);
						}
						::SetWindowLong(hWnd, DWL_MSGRESULT, 1);
						return TRUE;
					}
				}
				break;
			case EN_REQUESTRESIZE:
				::SetWindowPos(pParam->hwnd, 0, 0, 0, pParam->REQRESIZE.rc.right - pParam->REQRESIZE.rc.left, pParam->REQRESIZE.rc.bottom - pParam->REQRESIZE.rc.top, SWP_NOMOVE|SWP_NOZORDER);
				break;
			}
		}
		return TRUE;
	case WM_COMMAND:
		switch (wParam)
		{ 
		case IDCANCEL:
			::EndDialog(hWnd, wParam);
			break;
		}
		return TRUE;
	}
	return 0;
}

void CTraceWnd::OnInitDialog()
{
	ZeroMemory(&fr, sizeof fr);
	fr.lStructSize = sizeof(FINDREPLACE);
	fr.hwndOwner = hwndMain;
	fr.hInstance = g_hInstance;
	fr.lpstrFindWhat = fr.szFindWhat;
	fr.lpstrReplaceWith = fr.szReplaceWith;
	fr.wFindWhatLen = fr.wReplaceWithLen = fr.wBufLen;
	fr.Flags = FR_DOWN;
	hwndFind = 0;
	hwndEdit = ::GetDlgItem(hwndMain, IDC_ED_OUTPUT);
	::SendMessage(hwndEdit, EM_SETEVENTMASK, 0, dwDefaultEventMaskEdit);
	//::SendMessage(hwndEdit, EM_SETTEXTMODE, TM_SINGLELEVELUNDO, 0);
	LONG lStyle = ::GetWindowLong(hwndEdit, GWL_EXSTYLE);
	::SetWindowLong(hwndEdit, GWL_EXSTYLE, lStyle & ~WS_EX_NOPARENTNOTIFY);
	//ECO_SELECTIONBAR yields strange behavior...
	::SendMessage(hwndEdit, EM_SETOPTIONS, ECOOP_OR, ECO_SAVESEL);
	::SendMessage(hwndEdit, EM_LIMITTEXT, -1, 0);
	hwndHead = TabRuler_Create(hwndMain, hwndEdit, MAX_TAB_STOPS, 8);
	pAppProfile->ScanHeaderItemSizes(hwndHead, TEXT("TraceWnd"), TEXT("TabStops"));
	TabRuler_Update(hwndHead);
	SetDlgCaption(hwndMain, g_szAppName, MAKEINTRESOURCE(IDD_TRACE));
	HWND hCbFontFace = ::GetDlgItem(hwndMain, IDC_CB_FONTFACE);
	HWND hCbFontSize = ::GetDlgItem(hwndMain, IDC_CB_FONTSIZE);
	HDC hDC = ::GetDC(0);
	::EnumFonts(hDC, 0, PopulateCbFontFace, (LPARAM)hCbFontFace);
	::ReleaseDC(0, hDC);
	for (ATOM nAtom = 1 ; nAtom <= 99 ; ++nAtom)
	{
		TCHAR szAtom[8];
		GetAtomName(nAtom, szAtom, 8);
		ComboBox_AddString(hCbFontSize, szAtom + 1);
	}
	CHARFORMAT cf;
	cf.cbSize = sizeof cf;
	cf.dwMask = CFM_FACE | CFM_SIZE;
	pAppProfile->GetString(TEXT("TraceWnd"), TEXT("FontFace"), cf.szFaceName, LF_FACESIZE);
	cf.yHeight = pAppProfile->GetNumber(TEXT("TraceWnd"), TEXT("FontSize"), 0);
	::SendMessage(hwndEdit, (cf.yHeight *= 20) ? EM_SETCHARFORMAT : EM_GETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf);
	hwndTip = ::CreateInfoTip(hwndMain);
	ComboBox_SelectString(hCbFontFace, -1, cf.szFaceName);
	ComboBox_SetCurSel(hCbFontSize, cf.yHeight / 20 - 1);
}

void CTraceWnd::OnSelchangeCbFont()
{
	CHARFORMAT cf;
	cf.cbSize = sizeof cf;
	cf.dwMask = CFM_FACE | CFM_SIZE;
	::GetDlgItemText(hwndMain, IDC_CB_FONTFACE, cf.szFaceName, LF_FACESIZE);
	cf.yHeight = ::GetDlgItemInt(hwndMain, IDC_CB_FONTSIZE, 0, FALSE);
	pAppProfile->SetString(TEXT("TraceWnd"), TEXT("FontFace"), cf.szFaceName);
	pAppProfile->SetNumber(TEXT("TraceWnd"), TEXT("FontSize"), cf.yHeight);
	cf.yHeight *= 20;
	::SendMessage(hwndEdit, EM_SETCHARFORMAT, SCF_ALL, (LPARAM)&cf);
}

void CTraceWnd::OnFindReplace(FINDREPLACE *pfr)
{
	FINDTEXTEX ft;
	if (pfr->Flags & FR_FINDNEXT)
	{
		::SendMessage(hwndEdit, EM_EXGETSEL, 0, (LPARAM)&ft.chrg);
		if (pfr->Flags & FR_DOWN)
		{
			ft.chrg.cpMin = ft.chrg.cpMax;
			ft.chrg.cpMax = -1;
		}
		else
		{
			ft.chrg.cpMax = 0;
		}
		ft.lpstrText = pfr->lpstrFindWhat;
		if (::SendMessage(hwndEdit, EM_FINDTEXTEX, pfr->Flags & 7, (LPARAM)&ft) != -1)
		{
			::SendMessage(hwndEdit, EM_EXSETSEL, 0, (LPARAM)&ft.chrgText);
		}
	}
	else if (pfr->Flags & FR_DIALOGTERM)
	{
		hwndFind = 0;
		::SendMessage(hwndEdit, EM_SETOPTIONS, ECOOP_AND, ~ECO_NOHIDESEL);
	}
}

int NTAPI FormatNumberTip(LPCTSTR szText, LPTSTR pchVal)
{
	int iVal;
	return wsprintf(pchVal,
		StrToIntEx(szText, STIF_SUPPORT_HEX, &iVal) == FALSE ? TEXT("") :
		szText[1] != 'x' && szText[1] != 'X' ? TEXT("0x%x") :
		iVal >= 0 ? TEXT("%d") :
		TEXT("%d \x95 %u"), iVal, iVal);
}

LPTSTR NTAPI TranslateTip(LPCTSTR pchLine, LPCTSTR &pchName)
{
	int cchLine = lstrlen(pchLine);
	int cchName = pchName >= pchLine && pchName <= pchLine + cchLine ? 0 : lstrlen(pchName);
	if (CTokenizer *pTokenizer = CTokenizer::From(pchLine)
	-> EatKeyword(TEXT("INSERT")) -> EatKeyword(TEXT("INTO")))
	{
		pTokenizer = pTokenizer->EatSpace();
		int cchTableName = pTokenizer->LookAhead(-1);
		pTokenizer = pTokenizer[cchTableName].EatSpace();
		if (*pTokenizer->szText == '(')
		{
			CTokenizer *pclFields = pTokenizer;
			do
			{
				pTokenizer = pTokenizer[1].EatSpace();
				pTokenizer += pTokenizer->LookAhead();
			} while (*pTokenizer->szText == ',');
			if (*pTokenizer->szText == ')')
			{
				CTokenizer *pclValues = pTokenizer[1].EatKeyword(TEXT("VALUES"));
				if (pclValues)
				{
					pclValues = pclValues->EatSpace();
					if (*pclValues->szText != '(')
						pclValues = 0;
				}
				else
				{
					pclValues = pTokenizer[1].EatKeyword(TEXT("SELECT"));
					if (pclValues)
						--pclValues;
				}
				if (pclValues) do
				{
					pclFields = pclFields[1].EatSpace();
					pclValues = pclValues[1].EatSpace();
					int cchField = pclFields->LookAhead();
					int cchValue = pclValues->LookAhead();
					if (cchName == 0)
					{
						if (pchName >= pclFields->szText && pchName < pclFields->szText + cchField)
						{
							pchName = pclFields->szText;
							return pclFields->szText + cchField;
						}
					}
					else if (StrIsIntlEqual(TRUE, pclFields->szText, pchName, cchName) &&
						StrSpn(pclFields->szText + cchName, CTokenizer::szTrim) == cchField - cchName)
					{
						TCHAR szVal[43];
						szVal[0] = '\0';
						int cchAlloc = cchValue + 1;
						int cchExtra = FormatNumberTip(pclValues->szText, szVal + 3);
						if (cchExtra)
						{
							szVal[1] = '\x95'; // a bullet
							szVal[0] = szVal[2] = VK_SPACE;
							cchExtra += 3;
						}
						LPTSTR pszTip = (LPTSTR)CoTaskMemAlloc(cchAlloc + cchExtra);
						if (pszTip)
						{
							lstrcpyn(pszTip, pclValues->szText, cchAlloc);
							lstrcat(pszTip, szVal);
						}
						return pszTip;
					}
					pclFields += cchField;
					pclValues += cchValue;
				} while (*pclFields->szText == ',' && *pclValues->szText == ',');
			}
		}
	}
	return 0;
}

void CTraceWnd::ShowTip(BOOL bShow)
{
	POINT ptHit;
	if (!::GetCursorPos(&ptHit))
		return;
	::MapWindowPoints(0, hwndMain, &ptHit, 1);
	HWND hwndHit = ::ChildWindowFromPointEx(hwndMain, ptHit, CWP_SKIPINVISIBLE|CWP_SKIPTRANSPARENT);
	::MapWindowPoints(hwndMain, hwndHit, &ptHit, 1);
	int nCmdShow = SW_HIDE;
	if (bShow &&
		hwndHit == hwndEdit &&
		::GetFocus() == hwndHit &&
		::GetCursor() == ::LoadCursor(0, IDC_ARROW))
	{
		TEXTRANGE tr;
		::SendMessage(hwndEdit, EM_EXGETSEL , 0, (LPARAM)&tr);
		if (tr.chrg.cpMin < tr.chrg.cpMax)
		{
			LONG cpHit = ::SendMessage(hwndEdit, EM_CHARFROMPOS, 0, (LPARAM)&ptHit);
			LONG lnMin = ::SendMessage(hwndEdit, EM_EXLINEFROMCHAR, 0, tr.chrg.cpMin);
			LONG lnMax = ::SendMessage(hwndEdit, EM_EXLINEFROMCHAR, 0, tr.chrg.cpMax);
			LONG lnMid = ::SendMessage(hwndEdit, EM_EXLINEFROMCHAR, 0, tr.chrg.cpMax - 1);
			if (cpHit >= tr.chrg.cpMin && cpHit <= tr.chrg.cpMax && lnMin == lnMid)
			{
				if (lnMin != lnMax)
				{
					--tr.chrg.cpMax;
				}
				FINDTEXT ft;
				ft.chrg.cpMin = 0;
				ft.chrg.cpMax = tr.chrg.cpMax - tr.chrg.cpMin;
				ft.lpstrText = tr.lpstrText = (LPSTR)CoTaskMemAlloc(ft.chrg.cpMax + 1);
				if (ft.lpstrText)
				{
					TCHAR szVal[40];
					::SendMessage(hwndEdit, EM_GETTEXTRANGE, 0, (LPARAM)&tr);
					if (lnMin == lnMax)
					{
						// selection covers partial line
						if (int cch = FormatNumberTip(ft.lpstrText, szVal))
						{
							ft.lpstrText = szVal;
							ft.chrg.cpMax = cch;
						}
						else if (LPSTR pchLine = ::GetLineAtChar(hwndEdit, cpHit))
						{
							if (LPSTR pchTip = TranslateTip(pchLine, ft.lpstrText))
							{
								CoTaskMemFree(tr.lpstrText);
								ft.lpstrText = tr.lpstrText = pchTip;
								ft.chrg.cpMax = lstrlen(ft.lpstrText);
							}
							CoTaskMemFree(pchLine);
						}
					}
					GETTEXTLENGTHEX gtlx = { 0, CP_ACP };
					if (::SendMessage(hwndTip, EM_GETTEXTLENGTHEX, (WPARAM)&gtlx, 0) != ft.chrg.cpMax
					||	::SendMessage(hwndTip, EM_FINDTEXT, FR_DOWN, (LPARAM)&ft) != 0)
					{
						::ShowWindow(hwndTip, SW_HIDE);
						CHARFORMAT cf;
						cf.cbSize = sizeof cf;
						cf.dwMask = CFM_FACE | CFM_SIZE | CFM_OFFSET| CFM_BOLD | CFM_CHARSET;
						::SendMessage(hwndEdit, EM_GETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf);
						::SendMessage(hwndTip, EM_SETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf);
						::SetWindowText(hwndTip, ft.lpstrText);
						/*static int n = 0;
						char sz[12];
						wsprintf(sz, "%d", ++n);
						SetWindowText(GetActiveWindow(), sz);*/
					}
					nCmdShow = SW_SHOWNA;
					CoTaskMemFree(tr.lpstrText);
				}
			}
		}
	}
	::ShowWindow(hwndTip, nCmdShow);
	if (nCmdShow == SW_HIDE)
	{
		::SetWindowText(hwndTip, TEXT(""));
	}
}

static HMENU NTAPI GetContextMenu(int nPos)
{
	static HMENU hMenu = 0;
	if (hMenu == 0)
		hMenu = ::LoadMenu(g_hInstance, MAKEINTRESOURCE(IDM_CONTEXT));
	return ::GetSubMenu(hMenu, nPos);
}

static UINT NTAPI IsEditCmd(HWND hWnd, UINT nID)
{
	LONG cpLower = 0;
	LONG cpUpper = 0;
	LONG fReadOnly = ::GetWindowLong(hWnd, GWL_STYLE) & ES_READONLY;
	::SendMessage(hWnd, EM_GETSEL, (WPARAM)&cpLower, (LPARAM)&cpUpper);
	switch (nID)
	{
	case ID_TRACEWND_DELETE:
		if (!fReadOnly && cpLower != cpUpper)
			return WM_CLEAR;
		break;
	case ID_TRACEWND_CUT:
		if (!fReadOnly && cpLower != cpUpper)
			return WM_CUT;
		break;
	case ID_TRACEWND_COPY:
		if (cpLower != cpUpper)
			return WM_COPY;
		break;
	case ID_TRACEWND_PASTE:
		if (!fReadOnly && ::IsClipboardFormatAvailable(CF_TEXT))
			return WM_PASTE;
		break;
	case ID_TRACEWND_PASTEASSINGLELINE:
		if (!fReadOnly && ::IsClipboardFormatAvailable(CF_TEXT))
			return WM_COMMAND;
		break;
	case ID_TRACEWND_UNDO:
		if (!fReadOnly && ::SendMessage(hWnd, EM_CANUNDO, 0, 0))
			return EM_UNDO;
		break;
	case ID_TRACEWND_REDO:
		if (!fReadOnly && ::SendMessage(hWnd, EM_CANREDO, 0, 0))
			return EM_REDO;
		break;
	case ID_TRACEWND_SELECTALL:
		return WM_COMMAND;
	case ID_TRACEWND_JOINLINES:
		if (!fReadOnly && cpLower != cpUpper)
			return WM_COMMAND;
		break;
	}
	return 0;
}

void CTraceWnd::OnContextMenu(HWND hwndHit, int x, int y)
{
	enum
	{
		fAccept = MF_BYPOSITION|MF_ENABLED,
		fRefuse = MF_BYPOSITION|MF_GRAYED
	};
	UINT response = 0;
	HMENU hMenu = 0;
	if (hwndHit == hwndEdit)
	{
		hMenu = GetContextMenu(0);
		int nPos = ::GetMenuItemCount(hMenu);
		while (nPos > 0)
		{
			UINT nID = ::GetMenuItemID(hMenu, --nPos);
			::EnableMenuItem(hMenu, nPos, ::IsEditCmd(hwndHit, (WORD)nID) ? fAccept : fRefuse);
		}
	}
	if (hMenu)
	{
		response = (WORD)::TrackPopupMenu(hMenu, TPM_NONOTIFY|TPM_RETURNCMD, x, y, 0, hwndMain, 0);
	}
	switch (response)
	{
	case ID_TRACEWND_SELECTALL:
		::SendMessage(hwndHit, EM_SETSEL, 0, -1);
		break;
	case ID_TRACEWND_PASTEASSINGLELINE:
		::PasteAsSingleLine(hwndEdit, TRUE);
		break;
	case ID_TRACEWND_JOINLINES:
		::JoinSelectedLines(hwndEdit, TRUE);
		break;
	case 0:
		break;
	default:
		if (UINT uMsg = ::IsEditCmd(hwndHit, response))
		{
			ASSERT(uMsg != WM_COMMAND); // should have been handled by some case above
			::SendMessage(hwndHit, uMsg, 0, 0);
		}
	}
}

BOOL CALLBACK CTraceWnd::DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static CFloatState *FloatState = 0;
	static const LONG FloatScript[] =
	{
		IDC_ED_OUTPUT,			BY<1000>::X2R | BY<1000>::Y2B,
		IDC_TG_TOPMOST,			BY<1000>::X2L | BY<1000>::X2R,
		IDC_TG_DISABLE_TRACE,	BY<1000>::X2L | BY<1000>::X2R,
		IDC_PB_CLEAR,			BY<1000>::X2L | BY<1000>::X2R,
		IDCANCEL,				BY<1000>::X2L | BY<1000>::X2R,
		IDC_PB_OPTIONS,			BY<1000>::X2L | BY<1000>::X2R,
		IDC_PB_ABOUT,			BY<1000>::X2L | BY<1000>::X2R,
		0
	};
	POINT ptHit;
	switch (uMsg)
	{
	case WM_INITDIALOG:
		FloatState = new CFloatState(FloatScript);
		FloatState->Clear();
		hwndMain = hWnd;
		OnInitDialog();
		::SetForegroundWindow(hWnd);
		pAppProfile->ScanWindowPlacement(hWnd, TEXT("TraceWnd"), TEXT("Placement"));
		::SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(~IDC_PB_OPTIONS, 0), 0);
		return TRUE;
	case WM_SHOWWINDOW:
		if (wParam)
			::SetForegroundWindow(hWnd);
		return TRUE;
	case WM_NOTIFY:
		if (UNotify *pParam = reinterpret_cast<UNotify *>(lParam))
		{
			switch (pParam->NMHDR.code)
			{
			case NM_RELEASEDCAPTURE:
				if (pParam->hwnd == hwndHead)
				{
					pAppProfile->DumpHeaderItemSizes(hwndHead, TEXT("TraceWnd"), TEXT("TabStops"));
					::SendMessage(hwndEdit, EM_SETOPTIONS, ECOOP_AND, ~(ECO_AUTOHSCROLL|ECO_AUTOVSCROLL));
					TabRuler_Update(hwndHead);
					::SendMessage(hwndEdit, EM_SETOPTIONS, ECOOP_OR, ECO_AUTOHSCROLL|ECO_AUTOVSCROLL);
				}
				break;
			case EN_MSGFILTER:
				switch (pParam->MSGFILTER.msg)
				{
				case WM_RBUTTONDOWN:
					if (::GetCursorPos(&ptHit) && ::GetCursor() == ::LoadCursor(0, IDC_IBEAM))
					{
						::MapWindowPoints(0, hwndEdit, &ptHit, 1);
						CHARRANGE cr;
						cr.cpMin = cr.cpMax = ::SendMessage(hwndEdit, EM_CHARFROMPOS, 0, (LPARAM)&ptHit);
						::SendMessage(hwndEdit, EM_EXSETSEL, 0, (LPARAM)&cr);
					}
					break;
				case WM_LBUTTONDOWN:
					::SendMessage(hwndEdit, EM_SETEVENTMASK, 0, dwDefaultEventMaskEdit | ENM_SELCHANGE);
					ShowTip(FALSE);
					break;
				case WM_MOUSEMOVE:
				case WM_LBUTTONUP:
					ShowTip(TRUE);
					break;
				case WM_LBUTTONDBLCLK:
					::SendMessage(hwndEdit, EM_SETEVENTMASK, 0, dwDefaultEventMaskEdit);
					if (::GetCursorPos(&ptHit) &&
						::SendMessage(hwndEdit, EM_SELECTIONTYPE, 0, 0) == SEL_EMPTY)
					{
						::MapWindowPoints(0, hwndEdit, &ptHit, 1);
						LONG cpHit = ::SendMessage(hwndEdit, EM_CHARFROMPOS, 0, (LPARAM)&ptHit);
						LONG cpOrg = ::SendMessage(hwndEdit, EM_LINEFROMCHAR, cpHit, 0);
						cpOrg = ::SendMessage(hwndEdit, EM_LINEINDEX, cpOrg, 0);
						ASSERT(cpHit >= cpOrg);
						if (LPSTR pchOrg = ::GetLineAtChar(hwndEdit, cpOrg))
						{
							LPCSTR pchMin = pchOrg + cpHit - cpOrg;
							if (LPSTR pchMax = TranslateTip(pchOrg, pchMin))
							{
								::PostMessage(hwndEdit, EM_SETSEL, pchMin - pchOrg + cpOrg, pchMax - pchOrg + cpOrg);
							}
							CoTaskMemFree(pchOrg);
						}
					}
					break;
				default:
					if (pParam->MSGFILTER.msg >= WM_KEYFIRST &&
						pParam->MSGFILTER.msg < WM_IME_KEYLAST &&
						pParam->MSGFILTER.wParam < 0x100)
					{
						if (pParam->MSGFILTER.msg == WM_KEYUP &&
							::SendMessage(hwndEdit, EM_SELECTIONTYPE, 0, 0) == SEL_EMPTY)
						{
							ShowTip(FALSE);
						}
						::SendMessage(hwndEdit, EM_SETOPTIONS, ECOOP_OR, ECO_AUTOHSCROLL);
						WORD wKey = pParam->MSGFILTER.msg << 8 | pParam->MSGFILTER.wParam | ::GetModState() << 12;
						switch (wKey)
						{
						case MAKEWORD('F', WM_KEYDOWN | MOD_CONTROL << 4):
							if (hwndFind == 0)
							{
								::SendMessage(hwndEdit, EM_SETOPTIONS, ECOOP_OR, ECO_NOHIDESEL);
								hwndFind = ::FindText(&fr);
							}
							else
							{
								::SetFocus(hwndFind);
							}
							break;
						}
					}
					break;
				}
				break;
			case EN_SELCHANGE:
				if (::GetCapture() != hwndEdit)
				{
					::SendMessage(hwndEdit, EM_SETOPTIONS, ECOOP_OR, ECO_AUTOHSCROLL);
					::SendMessage(hwndEdit, EM_SETEVENTMASK, 0, dwDefaultEventMaskEdit);
					break;
				}
				// If selection is all entire lines, disable auto horizontal scroll
				if (pParam->SELCHANGE.seltyp != SEL_EMPTY &&
					::SendMessage(hwndEdit, EM_LINELENGTH, -1, 0) == 0)
				{
					::SendMessage(hwndEdit, EM_SETOPTIONS, ECOOP_AND, ~ECO_AUTOHSCROLL);
				}
				break;
			case EN_REQUESTRESIZE:
				::ResizeInfoTip(&pParam->REQRESIZE);
				break;
			}
		}
		return TRUE;
	case WM_SIZE:
	case WM_CAPTURECHANGED:
		if (::GetCapture() == 0)
		{
			pAppProfile->DumpWindowPlacement(hwndMain, TEXT("TraceWnd"), TEXT("Placement"));
		}
		break;
	case WM_NCDESTROY:
		delete FloatState;
		break;
	case WM_WINDOWPOSCHANGING:
		FloatState->Float(reinterpret_cast<WINDOWPOS *>(lParam));
		break;
	case WM_COMMAND:
		switch GET_WM_COMMAND_ID(wParam, lParam)
		{ 
		case IDC_PB_OPTIONS:
			if (DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_OPTIONS), hWnd, COptionsDlg::DlgProc) != IDOK)
				return TRUE;
			// fall through
		case GET_WM_COMMAND_ID(~IDC_PB_OPTIONS, 0):
			::SendMessage(hwndEdit, EM_SETBKGNDCOLOR, 0, rgbBkgnd);
			::SendMessage(hwndTip, EM_SETBKGNDCOLOR, 0, rgbPopup);
			if (ODBCTrace.fWaitForClose)
				HookProcessTermination();
			if (ODBCTrace.fCatchDebugStrings)
				HookOutputDebugString();
			::CheckDlgButton(hWnd, IDC_TG_TOPMOST, ODBCTrace.fTopMost);
			::CheckDlgButton(hWnd, IDC_TG_DISABLE_TRACE, ODBCTrace.fDisableTrace);
			// fall through
		case IDC_TG_TOPMOST:
		case IDC_TG_DISABLE_TRACE:
			ODBCTrace.fTopMost = ::IsDlgButtonChecked(hWnd, IDC_TG_TOPMOST);
			ODBCTrace.fDisableTrace = ::IsDlgButtonChecked(hWnd, IDC_TG_DISABLE_TRACE);
			::SetWindowPos(hWnd, ODBCTrace.fTopMost ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
			::SendMessage(hwndEdit, EM_SETREADONLY, !ODBCTrace.fDisableTrace, 0);
			if (ODBCTrace.fDisableTrace)
				::SendMessage(hwndEdit, EM_EMPTYUNDOBUFFER, 0, 0);
			return TRUE;
		case IDC_PB_CLEAR:
			::SetWindowText(hwndEdit, TEXT(""));
			return TRUE;
		case IDC_PB_ABOUT:
			DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_ABOUT), hWnd, DlgProcAbout);
			return TRUE;
		case IDCANCEL:
			if (::GetDlgItem(hWnd, IDCANCEL))
				::PostQuitMessage(0);
			else
				::ShowWindow(hWnd, SW_SHOWMINIMIZED);
			return TRUE;
		case IDC_ED_OUTPUT:
			switch GET_WM_COMMAND_CMD(wParam, lParam)
			{
			case EN_UPDATE:
				// If selection is all entire lines, disable auto horizontal scroll
				if (::SendMessage(hwndEdit, EM_SELECTIONTYPE, 0, 0) != SEL_EMPTY &&
					::SendMessage(hwndEdit, EM_LINELENGTH, -1, 0) == 0)
				{
					::SendMessage(hwndEdit, EM_SETOPTIONS, ECOOP_AND, ~ECO_AUTOHSCROLL);
				}
				else
				{
					::SendMessage(hwndEdit, EM_SETOPTIONS, ECOOP_OR, ECO_AUTOHSCROLL);
				}
				TabRuler_Scroll(hwndHead);
				break;
			case EN_SETFOCUS:
				ShowTip(TRUE);
				break;
			case EN_KILLFOCUS:
				ShowTip(FALSE);
				break;
			}
			return TRUE;
		case IDC_CB_FONTFACE:
		case IDC_CB_FONTSIZE:
			switch GET_WM_COMMAND_CMD(wParam, lParam)
			{
			case CBN_SELCHANGE:
				OnSelchangeCbFont();
				break;
			}
			return TRUE;
		}
		break;
	case WM_APP:
		if (CTraceMsg *pTraceMsg = reinterpret_cast<CTraceMsg *>(lParam))
		{
			pTraceMsg->Dump(hwndEdit);
			return TRUE;
		}
		break;
	//case WM_APP + 1:
	//	ODBCTracePrintf(rgbTlftm, "Sorry wasn't listening...\n");
	//	break;
	case WM_APP + DLL_THREAD_ATTACH:
		{
			CDateTimeString dts;
			ODBCTracePrintf(rgbTlftm, "<%lu> %s %s\n", wParam, dts.date, dts.time);
		}
		break;
	case WM_APP + DLL_THREAD_DETACH:
		{
			CPerformanceCounterString pcs = lParam;
			CDateTimeString dts;
			ODBCTracePrintf(rgbTlftm, "</%lu> %s %s {%s}\n", wParam, dts.date, dts.time, pcs.text);
		}
		break;
	case WM_GETMINMAXINFO:
		CFloatState::AdjustMax(hWnd, reinterpret_cast<MINMAXINFO *>(lParam));
		break;
	case WM_CONTEXTMENU:
		OnContextMenu((HWND)wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;
	default:
		if (uMsg == commdlg_FindReplace)
		{
			OnFindReplace(reinterpret_cast<FINDREPLACE *>(lParam));
		}
		break;
	}
	return 0;
}

void CTraceWnd::WaitForClose(LPCTSTR lpFormat, UINT uExitCode)
{
	if (ODBCTrace.fWaitForClose)
	{
		if (HWND hPbClear = ::GetDlgItem(hwndMain, IDC_PB_CLEAR))
		{
			TCHAR szText[1024];
			int cchText = ::GetWindowText(hwndMain, szText, RTL_NUMBER_OF(szText) - 40);
			wsprintf(szText + cchText, lpFormat, uExitCode);
			::SetWindowText(hwndMain, szText);
			::SetWindowLong(hPbClear, GWL_ID, IDCANCEL);
			::SetWindowText(hPbClear, TEXT("&Close"));
			::SetForegroundWindow(hwndMain);
		}
		::WaitForSingleObject(hThread, INFINITE);
	}
}

void WINAPI CTraceWnd::ExitProcess(UINT uExitCode)
{
	WaitForClose(TEXT(" - Exiting with code %u"), uExitCode);
	::ExitProcess(uExitCode);
}

void WINAPI CTraceWnd::TerminateProcess(HANDLE hProcess, UINT uExitCode)
{
	if (hProcess == GetCurrentProcess())
		WaitForClose(TEXT(" - Terminating with code %u"), uExitCode);
	::TerminateProcess(hProcess, uExitCode);
}

void CTraceWnd::HookProcessTermination(HMODULE hModule)
{
	CModuleDirectory dyImp(hModule);
	if (hModule != g_hInstance)
	{
		if (IMAGE_IMPORT_DESCRIPTOR *pIAT = dyImp.FindIAT("KERNEL32.DLL"))
		{
			dyImp.FindIATEntry(pIAT, ::ExitProcess) -> Patch(ExitProcess);
			dyImp.FindIATEntry(pIAT, ::TerminateProcess) -> Patch(TerminateProcess);
		}
	}
}

void CTraceWnd::HookProcessTermination()
{
	static LONG bDone = FALSE;
	if (::InterlockedExchange(&bDone, TRUE))
		return;
	if (TOOLHELP::DLL TOOLHELP = ::TOOLHELP)
	{
		HANDLE hSnapshot = TOOLHELP->CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, 0);
		TOOLHELP::MODULEENTRY32 me;
		me.dwSize = sizeof me;
		if (TOOLHELP->Module32First(hSnapshot, &me)) do
		{
			HookProcessTermination(me.hModule);
		} while (TOOLHELP->Module32Next(hSnapshot, &me));
		CloseHandle(hSnapshot);
	}
	else if (struct PSAPI *PSAPI = ::PSAPI)
	{
		HMODULE rghModule[801];
		rghModule[0] = 0;
		DWORD cbValid = 0;
		HANDLE hProcess = GetCurrentProcess();
		if (PSAPI->EnumProcessModules(hProcess, rghModule, sizeof rghModule, &cbValid))
		{
			if (cbValid >= sizeof rghModule)
			{
				// Well, can't help truncation...
				cbValid = sizeof rghModule - sizeof *rghModule;
			}
			rghModule[cbValid / sizeof *rghModule] = 0;
		}
		HMODULE *phModule = rghModule;
		while (HMODULE hModule = *phModule++) // 1st is EXE
		{
			HookProcessTermination(hModule);
		}
	}
	// Patch KERNEL32.DLL's EAT to catch subsequent bindings to ExitProcess()/TerminateProcess()
	HMODULE hModule = ::GetModuleHandle(TEXT("KERNEL32.DLL"));
	CModuleDirectory dyExp(hModule);
	dyExp.FindEATEntry("ExitProcess") -> Patch(dyExp->*CTraceWnd::ExitProcess);
	dyExp.FindEATEntry("TerminateProcess") -> Patch(dyExp->*CTraceWnd::TerminateProcess);
}

static void CALLBACK OnAfterOutputDebugStringA(HWND hWnd, UINT uMsg, DWORD dwData, LRESULT)
{
	TraceRaw *p = reinterpret_cast<TraceRaw *>(dwData);
	LocalFree(const_cast<LPTSTR>(p->text));
	delete p;
}

static void CALLBACK OnAfterOutputDebugStringW(HWND hWnd, UINT uMsg, DWORD dwData, LRESULT)
{
	TraceRaw *p = reinterpret_cast<TraceRaw *>(dwData);
	GlobalFreePtr(p->text);
	delete p;
}

void WINAPI CTraceWnd::OutputDebugStringA(LPCSTR lpOutputString)
{
	if (ODBCTrace.fCatchDebugStrings && !ODBCTrace.fDisableTrace)
		new TraceRaw(rgbDebug, StrDupA(lpOutputString), OnAfterOutputDebugStringA);
	if (ODBCTrace.fCatchDebugStrings != 1)
		::OutputDebugStringA(lpOutputString);
}

void WINAPI CTraceWnd::OutputDebugStringW(LPCWSTR lpOutputString)
{
	if (ODBCTrace.fCatchDebugStrings && !ODBCTrace.fDisableTrace)
		new TraceRaw(rgbDebug, GlobalVPrintf(CP_ACP, "%ls", (va_list)&lpOutputString),
			OnAfterOutputDebugStringW);
	if (ODBCTrace.fCatchDebugStrings != 1)
		::OutputDebugStringW(lpOutputString);
}

void CTraceWnd::HookOutputDebugString(HMODULE hModule)
{
	CModuleDirectory dyImp(hModule);
	if (hModule != g_hInstance)
	{
		if (IMAGE_IMPORT_DESCRIPTOR *pIAT = dyImp.FindIAT("KERNEL32.DLL"))
		{
			dyImp.FindIATEntry(pIAT, ::OutputDebugStringA) -> Patch(OutputDebugStringA);
			dyImp.FindIATEntry(pIAT, ::OutputDebugStringW) -> Patch(OutputDebugStringW);
		}
	}
}

void CTraceWnd::HookOutputDebugString()
{
	static LONG bDone = FALSE;
	if (::InterlockedExchange(&bDone, TRUE))
		return;
	if (TOOLHELP::DLL TOOLHELP = ::TOOLHELP)
	{
		HANDLE hSnapshot = TOOLHELP->CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, 0);
		TOOLHELP::MODULEENTRY32 me;
		me.dwSize = sizeof me;
		if (TOOLHELP->Module32First(hSnapshot, &me)) do
		{
			HookOutputDebugString(me.hModule);
		} while (TOOLHELP->Module32Next(hSnapshot, &me));
		CloseHandle(hSnapshot);
	}
	else if (struct PSAPI *PSAPI = ::PSAPI)
	{
		HMODULE rghModule[801];
		rghModule[0] = 0;
		DWORD cbValid = 0;
		HANDLE hProcess = GetCurrentProcess();
		if (PSAPI->EnumProcessModules(hProcess, rghModule, sizeof rghModule, &cbValid))
		{
			if (cbValid >= sizeof rghModule)
			{
				// Well, can't help truncation...
				cbValid = sizeof rghModule - sizeof *rghModule;
			}
			rghModule[cbValid / sizeof *rghModule] = 0;
		}
		HMODULE *phModule = rghModule;
		while (HMODULE hModule = *phModule++) // 1st is EXE
		{
			HookOutputDebugString(hModule);
		}
	}
	// Patch KERNEL32.DLL's EAT to catch subsequent bindings to OutputDebugString()
	HMODULE hModule = ::GetModuleHandle(TEXT("KERNEL32.DLL"));
	CModuleDirectory dyExp(hModule);
	dyExp.FindEATEntry("OutputDebugStringA") -> Patch(dyExp->*CTraceWnd::OutputDebugStringA);
	dyExp.FindEATEntry("OutputDebugStringW") -> Patch(dyExp->*CTraceWnd::OutputDebugStringW);
}

HWND NTAPI GetMDIOwner(HWND hWndParent)
{
	HWND hWndT;
	while ((::GetWindowLong(hWndParent, GWL_STYLE) & WS_CHILD) &&
		!(::GetWindowLong(hWndParent, GWL_EXSTYLE) & WS_EX_MDICHILD) &&
		(hWndT = ::GetParent(hWndParent)) != NULL)
	{
		hWndParent = hWndT;
	}
	return hWndParent;
}

void NTAPI LockLibrary(HMODULE hModule)
{
	TCHAR szPath[MAX_PATH];
	::GetModuleFileName(hModule, szPath, MAX_PATH);
	::LoadLibrary(szPath);
}

DWORD WINAPI CTraceWnd::ThreadProc(LPVOID)
{
	INITCOMMONCONTROLSEX icc = { sizeof(INITCOMMONCONTROLSEX), ICC_LISTVIEW_CLASSES };
	::InitCommonControlsEx(&icc);
	::CoInitialize(0);
	if FAILED(::CoCreateInstance(CLSID_OLEDB_CONVERSIONLIBRARY, 0,
		CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER, IID_IDataConvert, (void **)&ODBCTrace.pDataConvert))
	{
		::MessageBox(0, TEXT("IDataConvert is unavailable"), TEXT("ODBCTrace"), MB_ICONSTOP|MB_TOPMOST);
		return 0;
	}
	::LockLibrary(g_hInstance);
	::LoadLibrary(TEXT("RICHED20.DLL"));
	HWND hwndActive = ::GetForegroundWindow();
	ODBCTrace.uMsg = WM_APP;
	ODBCTrace.hWnd = ::CreateDialog(g_hInstance, MAKEINTRESOURCE(IDD_TRACE), 0, CTraceWnd::DlgProc);
	::SetEvent(hEventInitComplete);
	if (hwndActive)
	{
		DWORD dwProcessId = 0;
		::GetWindowThreadProcessId(hwndActive, &dwProcessId);
		if (dwProcessId == GetCurrentProcessId())
			::SetForegroundWindow(hwndActive);
	}
	MSG msg;
	while (::GetMessage(&msg, 0, 0, 0))
	{
		if (HWND hwndActive = ::GetMDIOwner(msg.hwnd))
			if (::IsDialogMessage(hwndActive, &msg))
				continue;
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}
	return 0;
}

void CTraceMsg::Send(SENDASYNCPROC lpCallBack)
{
	if (CTraceWnd::hEventInitComplete)
	{
		::WaitForSingleObject(CTraceWnd::hEventInitComplete, INFINITE);
	}
	if (ODBCTrace.hWnd)
	{
		if (lpCallBack == 0)
			::SendMessage(ODBCTrace.hWnd, ODBCTrace.uMsg, MSGID,
				reinterpret_cast<LPARAM>(this));
		else if (!::SendMessageCallback(ODBCTrace.hWnd, ODBCTrace.uMsg, MSGID,
				reinterpret_cast<LPARAM>(this), lpCallBack, reinterpret_cast<DWORD>(this)))
			lpCallBack(0, ODBCTrace.uMsg, reinterpret_cast<DWORD>(this), 0);
	}
}

static void NTAPI AppendText(HWND hwndEdit, CHARFORMAT *pcf, LPCSTR text)
{
	GETTEXTLENGTHEX gtlx = { 0, CP_ACP };
	CHARRANGE cr;
	cr.cpMax = ::SendMessage(hwndEdit, EM_GETTEXTLENGTHEX, (WPARAM)&gtlx, 0);
	if (cr.cpMax >= ODBCTrace.cpHighWaterMark)
	{
		// EM_SETSEL bounds no longer valid, so remove pending EM_SETSELs
		MSG msg;
		while (::PeekMessage(&msg, hwndEdit, EM_SETSEL, EM_SETSEL, PM_REMOVE)) { }
		cr.cpMax -= ODBCTrace.cpHighWaterMark - ODBCTrace.cpHighWaterMark / 4;
		cr.cpMax = ::SendMessage(hwndEdit, EM_EXLINEFROMCHAR, 0, cr.cpMax);
		cr.cpMax = ::SendMessage(hwndEdit, EM_LINEINDEX, cr.cpMax + 1, 0);
		cr.cpMin = 0;
		::SendMessage(hwndEdit, WM_SETREDRAW, 0, 0);
		::SendMessage(hwndEdit, EM_EXSETSEL, 0, (LPARAM)&cr);
		::SendMessage(hwndEdit, EM_REPLACESEL, 0, (LPARAM)L"");
		::SendMessage(hwndEdit, WM_SETREDRAW, 1, 0);
		::InvalidateRect(hwndEdit, 0, FALSE);
		cr.cpMax = ::SendMessage(hwndEdit, EM_GETTEXTLENGTHEX, (WPARAM)&gtlx, 0);
	}
	cr.cpMin = cr.cpMax;
	::SendMessage(hwndEdit, EM_EXSETSEL, 0, (LPARAM)&cr);
	::SendMessage(hwndEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)pcf);
	::SendMessage(hwndEdit, EM_REPLACESEL, 0, (LPARAM)text);
	// Empiric evidence shows that appending a large amount of text may leave
	// us with incomplete formatting. The fix is to explicitly EM_SETCHARFORMAT
	// chars beyond offset 3600:
	cr.cpMax = ::SendMessage(hwndEdit, EM_GETTEXTLENGTHEX, (WPARAM)&gtlx, 0);
	cr.cpMin += 3600;
	if (cr.cpMin < cr.cpMax)
	{
		::SendMessage(hwndEdit, EM_EXSETSEL, 0, (LPARAM)&cr);
		::SendMessage(hwndEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)pcf);
		cr.cpMin = cr.cpMax;
		::SendMessage(hwndEdit, EM_EXSETSEL, 0, (LPARAM)&cr);
	}
	::SendMessage(hwndEdit, WM_VSCROLL, SB_BOTTOM, 0);
}

IStream *NTAPI ODBCTraceGetStream(HWND hwndEdit)
{
	if (ODBCTrace.pStream)
	{
		static const LARGE_INTEGER lrgZero = { 0, 0 };
		ULARGE_INTEGER ulrgPosition = { 0, 0 };
		if (SUCCEEDED(ODBCTrace.pStream->Seek(lrgZero, STREAM_SEEK_CUR, &ulrgPosition)) &&
			(ulrgPosition.HighPart || ulrgPosition.LowPart >= (DWORD)ODBCTrace.cpHighWaterMarkFile))
		{
			ODBCTrace.pStream->Release();
			ODBCTrace.pStream = 0;
		}
	}
	if (ODBCTrace.pStream == 0)
	{
		TCHAR caption[1024];
		struct ofn : OPENFILENAME
		{
			TCHAR szFile[MAX_PATH];
		} ofn;
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = ::GetParent(hwndEdit);
		ofn.lpstrFile = ofn.szFile;
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrDefExt = CTraceWnd::szFileTitle + sizeof "ODBCTrace";
		ofn.lpstrTitle = caption;
		if (!pAppProfile->GetString(TEXT("LogFile"), g_szAppName, ofn.szFile, MAX_PATH))
		{
			lstrcpyn(ofn.szFile, g_szAppPath, g_szAppName - g_szAppPath);
			::PathAppend(ofn.szFile, CTraceWnd::szFileTitle);
		}
		LPTSTR complain = caption + wsprintf(caption, TEXT("%s - ODBCTrace Output File"), g_szAppName);
		do
		{
			if (ODBCTrace.fSendToFile == 2)
			{
				ODBCTrace.fSendToFile = 0;
				if (::GetSaveFileName(&ofn))
				{
					ODBCTrace.fSendToFile = 1;
					pAppProfile->SetString(TEXT("LogFile"), g_szAppName, ofn.szFile);
				}
			}
			if (ODBCTrace.fSendToFile == 1)
			{
				if (PathFileExists(ofn.szFile))
				{
					TCHAR szBackupFile[MAX_PATH];
					lstrcpy(szBackupFile, ofn.szFile);
					PathRenameExtension(szBackupFile, TEXT("--"));
					lstrcat(szBackupFile, PathFindExtension(ofn.szFile));
					DeleteFile(szBackupFile);
					MoveFile(ofn.szFile, szBackupFile);
				}
				if (FAILED(::SHCreateStreamOnFile(ofn.szFile, STGM_WRITE|STGM_SHARE_DENY_NONE|STGM_CREATE, &ODBCTrace.pStream)))
				{
					lstrcpy(complain, TEXT(" - Unable to create. Please retry."));
					ODBCTrace.fSendToFile = 2;
				}
			}
		} while (ODBCTrace.fSendToFile == 2);
	}
	return ODBCTrace.pStream;
}

void TraceRaw::Dump(HWND hwndEdit)
{
	if (ODBCTrace.fSendToWindow)
	{
		::AppendText(hwndEdit, &cf, text);
	}
	if (ODBCTrace.fSendToFile)
	{
		if (IStream *piStream = ODBCTraceGetStream(hwndEdit))
		{
			StreamPuts(piStream, text);
			piStream->Commit(STGC_DEFAULT);
		}
	}
}

void TraceFmtV::Dump(HWND hwndEdit)
{
	if (LPTSTR text = GlobalVPrintf(CP_ACP, fmt, arglist))
	{
		if (ODBCTrace.fSendToWindow)
		{
			::AppendText(hwndEdit, &cf, text);
		}
		if (ODBCTrace.fSendToFile)
		{
			if (IStream *piStream = ODBCTraceGetStream(hwndEdit))
			{
				StreamPuts(piStream, text);
				piStream->Commit(STGC_DEFAULT);
			}
		}
		GlobalFreePtr(text);
	}
}

LARGE_INTEGER CPerformanceCounterString::frequency;

CallFrame::CallFrame(const short function_id, const char *const function_name, SQLSMALLINT HandleType, SQLSMALLINT HandleOffset)
: pcs(0)
, function_id(function_id), function_name(function_name)
, HandleType(HandleType), HandleOffset(HandleOffset)
{
	ASSERT(GetCurrentThreadId() != CTraceWnd::dwThreadId);
	if (ODBCTrace.fSerialize)
		::EnterCriticalSection(pcs = &CallStack::cs);
}

CallFrame::~CallFrame()
{
	ASSERT(GetCurrentThreadId() != CTraceWnd::dwThreadId);
	if (pcs)
		::LeaveCriticalSection(pcs);
}

CRITICAL_SECTION CallStack::cs;

CallStack::CallStack(): head(0), depth(0), lockout(0)
{
	if (ODBCTrace.hWnd && ODBCTrace.fTraceThreadLifetime && !ODBCTrace.fDisableTrace)
	{
		::PostMessage(ODBCTrace.hWnd, ODBCTrace.uMsg + DLL_THREAD_ATTACH, CoGetCurrentProcess(), 0);
	}
	CPerformanceCounterString::GetCounter(time);
}

CallStack::~CallStack()
{
	if (ODBCTrace.hWnd && ODBCTrace.fTraceThreadLifetime && !ODBCTrace.fDisableTrace)
	{
		LONG lp = CPerformanceCounterString::QueryElapsed(time);
		::PostMessage(ODBCTrace.hWnd, ODBCTrace.uMsg + DLL_THREAD_DETACH, CoGetCurrentProcess(), lp);
	}
}

RETCODE CallStack::Push(CallFrame *frame)
{
	ASSERT(GetCurrentThreadId() != CTraceWnd::dwThreadId);
	if (frame == 0)
		return -1;
	::EnterCriticalSection(&cs);
	frame->next = head;
	head = frame;
	int depth = frame->depth = CallStack::depth++;
	CDateTimeString dts;
	ODBCTracePrintf(
		CTraceWnd::rgbEnter,
		"%lu[%d] %s %s ->%s\n",
		CoGetCurrentProcess(),
		frame->depth, dts.date, dts.time, frame->function_name);
	frame->Dump();
	CPerformanceCounterString::GetCounter(frame->time);
	::LeaveCriticalSection(&cs);
	return depth;
}

CallFrame *CallStack::Pop(RETCODE depth, RETCODE result)
{
	ASSERT(GetCurrentThreadId() != CTraceWnd::dwThreadId);
	if (depth == -1 || head == 0)
		return 0;
	::EnterCriticalSection(&cs);
	CallFrame *frame = head;
	head = head->next;
	CPerformanceCounterString pcs = CPerformanceCounterString::QueryElapsed(frame->time);
	CDateTimeString dts;
	ODBCTracePrintf(
		result >= 0 ? CTraceWnd::rgbLeave : CTraceWnd::rgbError,
		"%lu[%d] %s %s <-%s = %hd {%s}\n",
		CoGetCurrentProcess(),
		frame->depth, dts.date, dts.time, frame->function_name, result, pcs.text);
	CallStack::lockout = 1;
	// diagnostic info
	if (!ODBCTrace.fHideDiagInfo)
	{
		if (char *pszText = frame->DumpDiagFields(result))
		{
			TraceRaw(CTraceWnd::rgbSqlEx, pszText);
			GlobalFreePtr(pszText);
		}
	}
	if (result >= 0)
	{
		frame->DumpResults();
	}
	else if (frame->HandleType)
	{
		SQLHANDLE Handle = *(SQLHANDLE *)(LPBYTE(frame + 1) + frame->HandleOffset);
		SQLSMALLINT i = 0;
		SQLCHAR szSqlState[32];
		SQLCHAR szErrorMsg[1024];
		SQLINTEGER fNativeError = 0;
		SQLSMALLINT cbErrorMsg = 0;
		while SQL_SUCCEEDED(::SQLGetDiagRec(frame->HandleType, Handle, ++i,
			szSqlState, &fNativeError, szErrorMsg, sizeof szErrorMsg, &cbErrorMsg))
		{
			ODBCTracePrintf(CTraceWnd::rgbError, "\t%s = %#08lx = %s\n", szSqlState, fNativeError, szErrorMsg);
		}
	}
	CallStack::lockout = 0;
	--CallStack::depth;
	if (frame->depth != CallStack::depth)
	{
		int response = MessageBox(0, "depth mismatch", 0, MB_TOPMOST|MB_ICONSTOP|MB_OKCANCEL);
		if (response == IDCANCEL)
			ExitProcess(3);
	}
	::LeaveCriticalSection(&cs);
	return frame;
}

CallStack *CallStack::Ptr::operator->()
{
	return static_cast<CallStack*>(TlsGetValue(dwTlsIndex));
}

CallStack::Ptr stack;

void STDAPICALLTYPE ODBCTracePrintf(COLORREF crTextColor, LPCTSTR fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	TraceFmtV(crTextColor, fmt, args);
	va_end(args);
}

void STDAPIVCALLTYPE ODBCTraceVPrintf(COLORREF crTextColor, LPCTSTR fmt, va_list args)
{
	TraceFmtV(crTextColor, fmt, args);
}

RETCODE SQL_API TraceOpenLogFile(LPWSTR, LPWSTR, DWORD)
{
	return 0;
}

RETCODE SQL_API TraceCloseLogFile()
{
	return 0;
}

VOID SQL_API TraceReturn(RETCODE rethandle, RETCODE retcode)
{
	delete stack->Pop(rethandle, retcode);
} 

DWORD SQL_API TraceVersion()
{
	return TRACE_VERSION;
}

HMODULE CALLBACK ODBCTraceHookIntoSystemTracer()
{
	HMODULE hModule = 0;
	if (CTraceWnd::hThread && (hModule = ::LoadLibrary(TEXT("ODBCTRAC.DLL"))) != 0)
	{
		CModuleDirectory dyExp(hModule);

		dyExp.FindEATEntry("TraceOpenLogFile") -> Patch(dyExp->*TraceOpenLogFile);
		dyExp.FindEATEntry("TraceCloseLogFile") -> Patch(dyExp->*TraceCloseLogFile);
		dyExp.FindEATEntry("TraceReturn") -> Patch(dyExp->*TraceReturn);
		dyExp.FindEATEntry("TraceVersion") -> Patch(dyExp->*TraceVersion);

		const char *pchLower = ODBCTraceState;
		while (LPTSTR pchUpper = StrChrA(pchLower, ';'))
		{
			pchLower = StrRChrA(pchLower, pchUpper, VK_SPACE);
			static const char szTrace[] = "Trace";
			char szName[80];
			lstrcpynA(szName, szTrace, sizeof szName);
			lstrcpynA(szName + sizeof szTrace - 1, pchLower + 1, pchUpper - pchLower);
			szName[pchUpper - pchLower + sizeof szTrace - 1] = '\0';
			do
			{
				if (FARPROC fnSubstitute = ::GetProcAddress(g_hInstance, szName))
					dyExp.FindEATEntry(szName) -> Patch(dyExp->*fnSubstitute);
			} while (szName[pchUpper - pchLower + sizeof szTrace - 2] ^= 'W');
			pchLower = pchUpper + 1;
		}
	}
	return hModule;
}
