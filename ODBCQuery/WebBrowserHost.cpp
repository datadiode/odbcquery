/*/WebBrowserHost.cpp

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
#include <mshtmcid.h>
#include "WebBrowserHost.h"
#include "../Common/VersionData.h"

#pragma warning(disable: 4100)

static LPWSTR NTAPI LoadStringResource(HMODULE hModule, UINT uStringID)
{
	LPWSTR pwchMem = 0;
	HRSRC hResource = FindResourceEx(
		hModule, RT_STRING,
		MAKEINTRESOURCE(uStringID / 16 + 1),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT));
	if (hResource)
	{
		pwchMem = (LPWSTR)LoadResource(hModule, hResource);
		if (pwchMem)
		{
			while (uStringID & 15)
			{
				pwchMem += *pwchMem + 1;
				--uStringID;
			}
		}
	}
	return pwchMem;
}

CWebBrowserSite::CWebBrowserSite(COleControlContainer* pCtrlCont, CWebBrowserHost *pWebBrowserHost)
: COleControlSite(pCtrlCont), m_pWebBrowserHost(pWebBrowserHost)
{
	m_dwRef = 0;
	ASSERT(m_pWebBrowserHost->m_pWebBrowserSite == 0);
	m_pWebBrowserHost->m_pWebBrowserSite = this;
}

CWebBrowserSite::~CWebBrowserSite()
{
	ASSERT(m_pWebBrowserHost->m_pWebBrowserSite == this);
	m_pWebBrowserHost->m_pWebBrowserSite = 0;
	// NB: An ASSERT due to an unbalanced ExternalRelease() indicates incomplete
	// cleanup inside host window's OnDestroy(). See CDHtmlDialog::OnDestroy().
}

LPUNKNOWN CWebBrowserSite::GetInterfaceHook(const void *iid)
{
	LPOLESTR pszIID = 0;
	StringFromIID(*(IID *)iid, &pszIID);
	if (pszIID)
	{
		TRACE(_T("pszIID = %ls\n"), pszIID);
		CString strIID = pszIID;
		CoTaskMemFree(pszIID);
	}
	if (IsEqualIID(*(IID *)iid, IID_IOleCommandTarget))
		return static_cast<IOleCommandTarget *>(m_pWebBrowserHost);
	IID diid;
	if (IsEqualIID(*(IID *)iid, IID_IDispatch)
	||	m_pCtrlCont->m_pWnd->GetDispatchIID(&diid) && IsEqualIID(*(IID *)iid, diid))
		return static_cast<IDispatch *>(m_pWebBrowserHost);
	if (IsEqualIID(*(IID *)iid, IID_IDocHostUIHandler))
		return static_cast<IDocHostUIHandler *>(m_pWebBrowserHost);
	if (IsEqualIID(*(IID *)iid, IID_IDocHostShowUI))
		return static_cast<IDocHostShowUI *>(m_pWebBrowserHost);
	if (IsEqualIID(*(IID *)iid, IID_IInternetSecurityManager))
		return static_cast<IInternetSecurityManager *>(m_pWebBrowserHost);
	if (IsEqualIID(*(IID *)iid, IID_IElementBehaviorFactory))
		return static_cast<IElementBehaviorFactory *>(m_pWebBrowserHost);
	if (IsEqualIID(*(IID *)iid, IID_IServiceProvider)
	||	IsEqualIID(*(IID *)iid, IID_IUnknown))
		return static_cast<IServiceProvider *>(m_pWebBrowserHost);
	return 0;
}

void CWebBrowserSite::OnFinalRelease()
{
	LPDISPATCH pdispCmdTarget = m_pWebBrowserHost->m_pdispCmdTarget;
	COleControlSite::OnFinalRelease();
	if (pdispCmdTarget)
		delete CCmdTarget::FromIDispatch(pdispCmdTarget);
}

CWebBrowserHost::CWebBrowserHost()
: m_pWebBrowserSite(0)
, m_pdispCmdTarget(0)
{
}

CWebBrowserSite *CWebBrowserHost::CreateControl(COleControlContainer *pCont,
	CWnd *pWnd, DWORD dwStyle, const RECT& rect, UINT nID)
{
	CWebBrowserSite *pSite = new CWebBrowserSite(pCont, this);
	HRESULT hr = pSite->CreateControl(pWnd, CLSID_WebBrowser, 0, dwStyle, rect, nID);
	if (SUCCEEDED(hr))
	{
		ASSERT(pSite->m_hWnd);
#		if _MFC_VER >= 0x0700
		COleControlSiteOrWnd *pSiteOrWnd = new COleControlSiteOrWnd(pSite);
		pCont->m_listSitesOrWnds.AddTail(pSite->m_pSiteOrWnd = pSiteOrWnd);
#		endif
		pCont->m_siteMap.SetAt(pSite->m_hWnd, pSite);
		pWnd->SubclassWindow(pWnd->Detach());
	}
	else
	{
		delete pSite;
		pSite = 0;
	}
	return pSite;
}

void CWebBrowserHost::OnDestroy()
{
	// Tell the browser control we're shutting down
	if (m_pWebBrowserSite)
	{
		// Avoid immediate destruction of WebBrowserSite in VC7++
#		if _MFC_VER >= 0x0700
		ASSERT(m_pWebBrowserSite->m_pSiteOrWnd->m_pSite == m_pWebBrowserSite ||
			m_pWebBrowserSite->m_pSiteOrWnd->m_pSite == 0);
		m_pWebBrowserSite->m_pSiteOrWnd->m_pSite = 0;
#		endif
		// Avoid immediate destruction of WebBrowserSite in VC6
		m_pWebBrowserSite->m_pCtrlCont->m_siteMap.RemoveKey(m_pWebBrowserSite->m_hWnd);
		m_pWebBrowserSite->m_pObject->Close(OLECLOSE_NOSAVE);
	}
}

BOOL CWebBrowserHost::PreTranslateMessage(MSG *pMsg)
{
	if (m_pWebBrowserSite && m_pWebBrowserSite->m_pActiveObject &&
		m_pWebBrowserSite->m_pActiveObject->TranslateAccelerator(pMsg) == S_OK)
		return TRUE;
	// Avoid an occasional ASSERT after closing a binary behavior's popup
	if (!::IsWindow(pMsg->hwnd))
		return TRUE;
	return FALSE;
}

// IUnknown methods
STDMETHODIMP CWebBrowserHost::QueryInterface(REFIID riid, void **ppvObject)
{
	IUnknown *punk = 0;
	if (m_pWebBrowserSite)
		punk = m_pWebBrowserSite->GetInterfaceHook(&riid);
	else if (IsEqualIID(riid, IID_IUnknown))
		punk = static_cast<IServiceProvider *>(this);
	*ppvObject = punk;
	if (punk == 0)
		return E_NOINTERFACE;
	AddRef();
	return S_OK;
}

STDMETHODIMP_(ULONG) CWebBrowserHost::AddRef()
{
	return m_pWebBrowserSite->ExternalAddRef();
}

STDMETHODIMP_(ULONG) CWebBrowserHost::Release()
{
	return m_pWebBrowserSite->ExternalRelease();
}

// IOleCommandTarget methods
STDMETHODIMP CWebBrowserHost::QueryStatus(const GUID *pguidCmdGroup,
	ULONG cCmds, OLECMD prgCmds[], OLECMDTEXT *pCmdText)
{
	return S_OK;
}

STDMETHODIMP CWebBrowserHost::Exec(const GUID *pguidCmdGroup,
	DWORD nCmdID, DWORD nCmdexecopt, VARIANT* pvaIn, VARIANT* pvaOut)
{
	HRESULT hr = OLECMDERR_E_NOTSUPPORTED;
	if (pguidCmdGroup != 0)
	{
		hr = OLECMDERR_E_UNKNOWNGROUP;
	}
	return hr;
}

// IDispatch methods
STDMETHODIMP CWebBrowserHost::GetTypeInfoCount(UINT *pctinfo)
{
	return m_pdispCmdTarget->GetTypeInfoCount(pctinfo);
}

STDMETHODIMP CWebBrowserHost::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo)
{
	return m_pdispCmdTarget->GetTypeInfo(iTInfo, lcid, ppTInfo);
}

STDMETHODIMP CWebBrowserHost::GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId)
{
	return m_pdispCmdTarget->GetIDsOfNames(riid, rgszNames, cNames, lcid, rgDispId);
}

STDMETHODIMP CWebBrowserHost::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags,
	DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
{
	return m_pdispCmdTarget->Invoke(dispIdMember, riid, lcid,
		wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr);
}

// IDocHostUIHandler methods
STDMETHODIMP CWebBrowserHost::ShowContextMenu(DWORD dwID, POINT *ppt, IUnknown *pcmdtReserved, IDispatch *pdispReserved)
{
	return S_FALSE;
}

STDMETHODIMP CWebBrowserHost::GetHostInfo(DOCHOSTUIINFO *pInfo)
{
	return E_NOTIMPL;
}

STDMETHODIMP CWebBrowserHost::ShowUI(DWORD dwID, IOleInPlaceActiveObject *pActiveObject, IOleCommandTarget *pCommandTarget, IOleInPlaceFrame *pFrame, IOleInPlaceUIWindow *pDoc)
{
	return S_FALSE;
}

STDMETHODIMP CWebBrowserHost::HideUI(void)
{
	return E_NOTIMPL;
}

STDMETHODIMP CWebBrowserHost::UpdateUI(void)
{
	return E_NOTIMPL;
}

STDMETHODIMP CWebBrowserHost::EnableModeless(BOOL fEnable)
{
	return E_NOTIMPL;
}

STDMETHODIMP CWebBrowserHost::OnDocWindowActivate(BOOL fActivate)
{
	return E_NOTIMPL;
}

STDMETHODIMP CWebBrowserHost::OnFrameWindowActivate(BOOL fActivate)
{
	return E_NOTIMPL;
}

STDMETHODIMP CWebBrowserHost::ResizeBorder(LPCRECT prcBorder, IOleInPlaceUIWindow *pUIWindow, BOOL fRameWindow)
{
	return E_NOTIMPL;
}

STDMETHODIMP CWebBrowserHost::TranslateAccelerator(LPMSG lpMsg, const GUID *pguidCmdGroup, DWORD nCmdID)
{
	return E_NOTIMPL;
}

STDMETHODIMP CWebBrowserHost::GetOptionKeyPath(LPOLESTR *pchKey, DWORD dw)
{
	HMODULE hModule = GetModuleHandle(0);
	const CVersionData *pvd = pvd->Load(hModule)->Find(L"StringFileInfo")->First();
	LPCWSTR CompanyName = pvd->Find(L"CompanyName")->Data();
	LPCWSTR InternalName = pvd->Find(L"InternalName")->Data();
	if (CompanyName == 0 || InternalName == 0)
		return E_NOTIMPL;
	WCHAR wszKey[MAX_PATH];
	wnsprintfW(wszKey, MAX_PATH, L"Software\\%s\\%s\\WebBrowserHost", CompanyName, InternalName);
	*pchKey = AfxAllocTaskWideString(wszKey);
	if (*pchKey == 0)
		return E_OUTOFMEMORY;
	return S_OK;
}

STDMETHODIMP CWebBrowserHost::GetDropTarget(IDropTarget *pDropTarget, IDropTarget **ppDropTarget)
{
	*ppDropTarget = NULL;
	return E_NOTIMPL;
}

STDMETHODIMP CWebBrowserHost::GetExternal(IDispatch **ppDispatch)
{
	if (m_pdispCmdTarget)
	{
		AddRef();
		*ppDispatch = this;
		return S_OK;
	}
	*ppDispatch = NULL;
	return E_NOTIMPL;
}

STDMETHODIMP CWebBrowserHost::TranslateUrl(DWORD dwTranslate, OLECHAR *pchURLIn, OLECHAR **ppchURLOut)
{
	*ppchURLOut = NULL;
	return S_FALSE;
}

STDMETHODIMP CWebBrowserHost::FilterDataObject(IDataObject *pDO, IDataObject **ppDORet)
{
	*ppDORet = NULL;
	return S_FALSE;
}

// IInternetSecurityManager methods
STDMETHODIMP CWebBrowserHost::SetSecuritySite(IInternetSecurityMgrSite *pSite)
{
	return INET_E_DEFAULT_ACTION;
}

STDMETHODIMP CWebBrowserHost::GetSecuritySite(IInternetSecurityMgrSite **ppSite)
{
	return INET_E_DEFAULT_ACTION;
}

STDMETHODIMP CWebBrowserHost::MapUrlToZone(LPCWSTR pwszUrl, DWORD *pdwZone, DWORD dwFlags)
{
	return INET_E_DEFAULT_ACTION;
}

STDMETHODIMP CWebBrowserHost::GetSecurityId(
	LPCWSTR pwszUrl,
	BYTE *pbSecurityId,
	DWORD *pcbSecurityId, 
	DWORD dwReserved)
{
	return INET_E_DEFAULT_ACTION;
}

STDMETHODIMP CWebBrowserHost::ProcessUrlAction(
	/* [in] */ LPCWSTR pwszUrl,
	/* [in] */ DWORD dwAction,
	/* [size_is][out] */ BYTE *pPolicy,
	/* [in] */ DWORD cbPolicy,
	/* [in] */ BYTE *pContext,
	/* [in] */ DWORD cbContext,
	/* [in] */ DWORD dwFlags,
	/* [in] */ DWORD dwReserved)
{
	if (dwAction >= URLACTION_ACTIVEX_MIN && dwAction <= URLACTION_ACTIVEX_MAX
	||	dwAction >= URLACTION_JAVA_MIN && dwAction <= URLACTION_JAVA_MAX
	||	dwAction >= URLACTION_SCRIPT_MIN && dwAction <= URLACTION_SCRIPT_MAX
	||	dwAction == URLACTION_CROSS_DOMAIN_DATA)
	{
		if (cbPolicy >= sizeof(DWORD))
		{
			*(DWORD *)pPolicy = URLPOLICY_ALLOW | URLPOLICY_DONTCHECKDLGBOX;
			return S_OK;
		}
	}
	return INET_E_DEFAULT_ACTION;
}

STDMETHODIMP CWebBrowserHost::QueryCustomPolicy(
	LPCWSTR pwszUrl,
	REFGUID guidKey,
	BYTE **ppPolicy,
	DWORD *pcbPolicy,
	BYTE *pContext,
	DWORD cbContext,
	DWORD dwReserved)
{
	return INET_E_DEFAULT_ACTION;
}

STDMETHODIMP CWebBrowserHost::SetZoneMapping(
	DWORD dwZone,
	LPCWSTR lpszPattern,
	DWORD dwFlags)
{
	return INET_E_DEFAULT_ACTION;
}

STDMETHODIMP CWebBrowserHost::GetZoneMappings(
	DWORD dwZone,
	IEnumString **ppenumString, 
	DWORD dwFlags)
{
	return INET_E_DEFAULT_ACTION;
}

STDMETHODIMP CWebBrowserHost::ShowMessage(
	/* [in] */ HWND hwnd,
	/* [in] */ LPOLESTR lpstrText,
	/* [in] */ LPOLESTR lpstrCaption,
	/* [in] */ DWORD dwType,
	/* [in] */ LPOLESTR lpstrHelpFile,
	/* [in] */ DWORD dwHelpContext,
	/* [out] */ LRESULT *plResult)
{
	if (HMODULE hDLL = GetModuleHandle(TEXT("SHDOCLC.DLL")))
	{
		// 2213 identifies window caption "Microsoft Internet Explorer" (as per Platform SDK)
		if (LPWSTR pwsz = LoadStringResource(hDLL, 2213))
		{
			if (StrCmpNW(lpstrCaption, pwsz + 1, *pwsz) == 0 && lpstrCaption[*pwsz] == '\0')
			{
				const CVersionData *pvd = pvd->Load()->Find(L"StringFileInfo")->First();
				lpstrCaption = const_cast<LPOLESTR>(pvd->Find(L"InternalName")->Data());
			}
		}
	}
	while (hwnd && !::IsWindowVisible(hwnd))
		hwnd = ::GetParent(hwnd);
	*plResult = ::MessageBoxW(hwnd, lpstrText, lpstrCaption, dwType | MB_TASKMODAL);
	return S_OK;
}

STDMETHODIMP CWebBrowserHost::ShowHelp(
	/* [in] */ HWND hwnd,
	/* [in] */ LPOLESTR pszHelpFile,
	/* [in] */ UINT uCommand,
	/* [in] */ DWORD dwData,
	/* [in] */ POINT ptMouse,
	/* [out] */ IDispatch *pDispatchObjectHit)
{
	return INET_E_DEFAULT_ACTION;
}

// IElementBehaviorFactory methods
STDMETHODIMP CWebBrowserHost::FindBehavior(
	/* [in] */ BSTR bstrBehavior,
	/* [in] */ BSTR bstrBehaviorUrl,
	/* [in] */ IElementBehaviorSite *pSite,
	/* [out][retval] */ IElementBehavior **ppBehavior)
{
	return E_NOTIMPL;
}

STDMETHODIMP CWebBrowserHost::QueryService(
	REFGUID guidService,  
	REFIID riid,
	void **ppvObject)
{
	if (guidService == SID_SElementBehaviorFactory &&
		riid == IID_IElementBehaviorFactory)
	{
		return QueryInterface(riid, ppvObject);
	}
	if (guidService == SID_SInternetSecurityManager &&
		riid == IID_IInternetSecurityManager)
	{
		return QueryInterface(riid, ppvObject);
	}
	*ppvObject = NULL;
	return E_NOINTERFACE;
}
