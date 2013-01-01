#pragma once

#include <transact.h>

#if _MFC_VER < 0x0700
#include "../src/occimpl.h"
#endif

class CWebBrowserSite;

class CWebBrowserHost
: public IOleCommandTarget
, public IDispatch
, public IDocHostUIHandler
, public IInternetSecurityManager
, public IDocHostShowUI
, public IElementBehaviorFactory
, public IServiceProvider
{
public:
// IUnknown methods
	STDMETHOD(QueryInterface)(REFIID, void **);
	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();
// IDispatch methods
	STDMETHOD(GetTypeInfoCount)(UINT *pctinfo);
	STDMETHOD(GetTypeInfo)(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo);
	STDMETHOD(GetIDsOfNames)(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId);
	STDMETHOD(Invoke)(
		DISPID dispIdMember,
		REFIID riid,
		LCID lcid,
		WORD wFlags,
		DISPPARAMS *pDispParams,
		VARIANT *pVarResult,
		EXCEPINFO *pExcepInfo,
		UINT *puArgErr);
// IOleCommandTarget methods
	STDMETHOD(QueryStatus)(
		/* [unique][in] */ const GUID *pguidCmdGroup,
		/* [in] */ ULONG cCmds,
		/* [out][in][size_is] */ OLECMD prgCmds[],
		/* [unique][out][in] */ OLECMDTEXT *pCmdText);
	STDMETHOD(Exec)(
		/* [unique][in] */ const GUID *pguidCmdGroup,
		/* [in] */ DWORD nCmdID,
		/* [in] */ DWORD nCmdexecopt,
		/* [unique][in] */ VARIANT *pvaIn,
		/* [unique][out][in] */ VARIANT *pvaOut);
// IDocHostUIHandler methods
	STDMETHOD(ShowContextMenu)(DWORD dwID, POINT *ppt, IUnknown *pcmdtReserved, IDispatch *pdispReserved);
	STDMETHOD(GetHostInfo)(DOCHOSTUIINFO *pInfo);
	STDMETHOD(ShowUI)(DWORD dwID, IOleInPlaceActiveObject *pActiveObject, IOleCommandTarget *pCommandTarget, IOleInPlaceFrame *pFrame, IOleInPlaceUIWindow *pDoc);
	STDMETHOD(HideUI)(void);
	STDMETHOD(UpdateUI)(void);
	STDMETHOD(EnableModeless)(BOOL fEnable);
	STDMETHOD(OnDocWindowActivate)(BOOL fActivate);
	STDMETHOD(OnFrameWindowActivate)(BOOL fActivate);
	STDMETHOD(ResizeBorder)(LPCRECT prcBorder, IOleInPlaceUIWindow *pUIWindow, BOOL fRameWindow);
	STDMETHOD(TranslateAccelerator)(LPMSG lpMsg, const GUID *pguidCmdGroup, DWORD nCmdID);
	STDMETHOD(GetOptionKeyPath)(LPOLESTR *pchKey, DWORD dw);
	STDMETHOD(GetDropTarget)(IDropTarget *pDropTarget, IDropTarget **ppDropTarget);
	STDMETHOD(GetExternal)(IDispatch **ppDispatch);
	STDMETHOD(TranslateUrl)(DWORD dwTranslate, OLECHAR *pchURLIn, OLECHAR **ppchURLOut);
	STDMETHOD(FilterDataObject)(IDataObject *pDO, IDataObject **ppDORet);
// IInternetSecurityManager methods
	STDMETHOD(SetSecuritySite)(IInternetSecurityMgrSite *);
	STDMETHOD(GetSecuritySite)(IInternetSecurityMgrSite **);
	STDMETHOD(MapUrlToZone)(LPCWSTR, DWORD *, DWORD);
	STDMETHOD(GetSecurityId)(LPCWSTR, BYTE *, DWORD *, DWORD);
	STDMETHOD(ProcessUrlAction)(LPCWSTR, DWORD, BYTE *, DWORD, BYTE *, DWORD, DWORD, DWORD);
	STDMETHOD(QueryCustomPolicy)(LPCWSTR, REFGUID, BYTE **, DWORD *, BYTE *, DWORD, DWORD);
	STDMETHOD(SetZoneMapping)(DWORD, LPCWSTR, DWORD);
	STDMETHOD(GetZoneMappings)(DWORD, IEnumString **, DWORD);
// IDocHostShowUI methods
	STDMETHOD(ShowMessage)(
		/* [in] */ HWND hwnd,
		/* [in] */ LPOLESTR lpstrText,
		/* [in] */ LPOLESTR lpstrCaption,
		/* [in] */ DWORD dwType,
		/* [in] */ LPOLESTR lpstrHelpFile,
		/* [in] */ DWORD dwHelpContext,
		/* [out] */ LRESULT *plResult);
	STDMETHOD(ShowHelp)(
		/* [in] */ HWND hwnd,
		/* [in] */ LPOLESTR pszHelpFile,
		/* [in] */ UINT uCommand,
		/* [in] */ DWORD dwData,
		/* [in] */ POINT ptMouse,
		/* [out] */ IDispatch *pDispatchObjectHit);
// IElementBehaviorFactory methods
	STDMETHOD(FindBehavior)(
		/* [in] */ BSTR bstrBehavior,
		/* [in] */ BSTR bstrBehaviorUrl,
		/* [in] */ IElementBehaviorSite *pSite,
		/* [out][retval] */ IElementBehavior **ppBehavior);
// IServiceProvider methods
	STDMETHOD(QueryService)(REFGUID, REFIID, void **);
// Data
	CWebBrowserSite *m_pWebBrowserSite;
	IDispatch *m_pdispCmdTarget;
protected:
	CWebBrowserHost();
	CWebBrowserSite *CreateControl(COleControlContainer *, CWnd *, DWORD, const RECT &, UINT);
	void OnDestroy();
	BOOL PreTranslateMessage(MSG *);
// IE7 
	enum DOCHOSTUIFLAG_IE7
	{
		DOCHOSTUIFLAG_HOST_NAVIGATES = 0x02000000,
		DOCHOSTUIFLAG_ENABLE_REDIRECT_NOTIFICATION = 0x04000000,
		DOCHOSTUIFLAG_USE_WINDOWLESS_SELECTCONTROL = 0x08000000,
		DOCHOSTUIFLAG_USE_WINDOWED_SELECTCONTROL = 0x10000000
	};
};

class CWebBrowserSite: public COleControlSite
{
	friend CWebBrowserHost;
public:
	CWebBrowserHost *m_pWebBrowserHost;
	CWebBrowserSite(COleControlContainer *, CWebBrowserHost *);
	~CWebBrowserSite();
protected:
// Implementation
	LPUNKNOWN GetInterfaceHook(const void *iid);
	virtual void OnFinalRelease();
#	if _MFC_VER >= 0x0700
	COleControlSiteOrWnd *m_pSiteOrWnd;
#	endif
};
