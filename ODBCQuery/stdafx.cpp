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

inline CRuntimeClass *GetRuntimeClass(CListCtrl *p)
{
	return p->CListCtrl::GetRuntimeClass();
}

template<class T>
T mfc_dynamic_cast(CObject *pObject)
{
	CRuntimeClass *pRuntimeClass = GetRuntimeClass(static_cast<T>(pObject));
	return static_cast<T>(AfxDynamicDownCast(pRuntimeClass, pObject));
}

#define dynamic_cast mfc_dynamic_cast

#if _MFC_VER == 0x0421

inline void AfxCheckError(HRESULT hr)
{
	if (FAILED(hr))
		AfxThrowOleException(hr);
}

namespace MFCX
{
	class CHeaderCtrl : public ::CHeaderCtrl
	{
	public:
		int HitTest(HDHITTESTINFO *phdhti)
		{
			return SendMessage(HDM_HITTEST, 0, reinterpret_cast<LPARAM>(phdhti));
		}
	};

	class CListCtrl : public ::CListCtrl
	{
	public:
		CHeaderCtrl *GetHeaderCtrl()
		{
			using ::SendMessage;
			return static_cast<CHeaderCtrl *>(FromHandle(ListView_GetHeader(m_hWnd)));
		}
		int SubItemHitTest(LVHITTESTINFO *plvhti)
		{
			using ::SendMessage;
			return ListView_SubItemHitTest(m_hWnd, plvhti);
		}
	};
}

#define CHeaderCtrl MFCX::CHeaderCtrl
#define CListCtrl MFCX::CListCtrl

#endif // _MFC_VER == 0x0421
