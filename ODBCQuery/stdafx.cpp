#define VC_EXTRALEAN		// Selten verwendete Teile der Windows-Header nicht einbinden
#define _CRT_SECURE_NO_WARNINGS
#define _USE_32BIT_TIME_T
#define __JAK__				// JAK 20000221 (MFC6)
#define _WIN32_WINNT 0x0501

#include <afxwin.h>			// MFC-Kern- und -Standardkomponenten
#include <afxext.h>			// MFC-Erweiterungen
#include <afxdb.h>			// MFC ODBC-Datenbankklassen
#include <sqlext.h>			// MFC ODBC-Datenbankklassen
#include <afxcmn.h>
#include <afxole.h>
#include <afxpriv.h>		// CRecentFileList, CSharedFile
#include <afxtempl.h>
#include <afxinet.h>
#include <shlwapi.h>
#include <mshtml.h>
#include <mshtmhst.h>
#include <exdisp.h>

#include <atlbase.h>
#include <wincrypt.h>

#include "../scintilla/include/Platform.h"
#include "../scintilla/include/Scintilla.h"
#include "../scintilla/include/SciLexer.h"

#include <assert.h>

typedef CString CStringA;

#ifndef AFXASSUME
#define AFXASSUME assert
#endif

#include "ScintillaDocView.h"
