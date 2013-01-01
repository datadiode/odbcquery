#include "../Common/FloatState.h"
#include "../Common/SplitState.h"
#include "WebBrowserHost.h"

class CChildFrame
: public CMDIChildWnd
, public CWebBrowserHost
, public CFloatState
, public CSplitState
{
	DECLARE_DYNCREATE(CChildFrame)
public:
	CChildFrame();

// Attribute
public:
	CTabCtrl m_TcDictionary;
	CListCtrl m_LvTables;
	CListCtrl m_LvProcedures;
	CTabCtrl m_TcScripts;
	CWnd m_wndBrowser;
	CTabCtrl m_TcResults;
	CScintillaCtrl m_EdLog;
	CComboBox m_CbUse;

	CMapStringToPtr m_rgTypeTables;
	CMapStringToPtr m_rgOwnerTables;
	CMapStringToPtr m_rgOwnerProcedures;

	CString m_strStatus;
private:
	static ATOM m_cwTabCtrl;
	static ATOM m_cwComboBox;
	HWND m_hWndFocus;
	IWebBrowser2 *m_pBrowserApp;
	//IPropertyPage *m_pPropertyPage;
	int m_iLogStyle;
	// Operationen
public:
	CODBCQueryDoc *GetDocument()
	{
		return static_cast<CODBCQueryDoc *>(GetActiveDocument());
	}
	CODBCQueryDocTemplate *GetDocTemplate()
	{
		return static_cast<CODBCQueryDocTemplate *>(GetActiveDocument()->GetDocTemplate());
	}
	CDatabaseEx *GetDatabase()
	{
		return &static_cast<CODBCQueryDoc *>(GetActiveDocument())->m_db;
	}
	void OnInitialUpdate();
	void DeleteDataPages(int iImageMax = 0);
	BOOL ClearResults();
	void AppendSql(LPCTSTR);
	void AppendLog(LPCTSTR);
	void AppendLogStyled(LPCTSTR);
	void LoadStyles(CScintillaCtrl &);
// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CChildFrame)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnUpdateFrameTitle(BOOL bAddToTitle);
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void PostNcDestroy();
	virtual void OnFinalRelease();
	public:
	virtual void RecalcLayout(BOOL bNotify = TRUE);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementierung
public:
	virtual ~CChildFrame();
	int AppendLog(CException *, UINT = 0);
	void PopulateCbUse();
	void PopulateWb();
	void PopulateLvTables();
	void PopulateLvProcedures();
	void PopulateTcDictionary();
protected:
	CScintillaCtrl *CreateTextPage(LPCTSTR);
	BOOL TrackPopupMenu(CMapStringToPtr &, CPoint, BOOL bAllowSubset = FALSE);
	void RetrieveText(CListCtrl &, BOOL bOnePage = FALSE);
	void RetrievePrimaryKeys(BOOL bOnePage = FALSE);
	void RetrieveForeignKeys(BOOL bOnePage = FALSE);
	void RetrieveTablePrivileges(BOOL bOnePage = FALSE);
	void RetrieveStatistics(int fUnique, int fAccuracy, BOOL bOnePage = FALSE);
	void RetrieveColumns(BOOL bOnePage = FALSE);
	void RetrieveSpecialColumns(int type, int scope, int nullable, BOOL bOnePage = FALSE);
	void RetrieveProcedureColumns(BOOL bOnePage = FALSE);
	void RetrieveDataTypes(int fSqlType);
	void RetrieveDBInfo(BOOL bOnePage = FALSE);
	void DropSelectedTables();
	void ChangeOwnerSelectedTables();
	void GrantAllSelectedTables(BOOL);
	void EnableIdentity(BOOL);
	void OnContextMenuLvTables(CPoint);
	void OnContextMenuLvProcedures(CPoint);
	void OnContextMenuCbUse(CPoint);
	void GenerateSQL(CListCtrl *);
	void DeleteRows(CListCtrl *);
	void UpdateBlob(CListCtrl *, int, int, BSTR);
	void TextBox(CListCtrl *, int, int);
	void HexBox(CListCtrl *, int, int);
	void CreateExcelDocument(CListCtrl *);
	void OnContextMenuTcResults(CPoint);
	void OnContextMenuLvResults(CListCtrl *, CPoint);
	void PopulatePage(CWnd *);
	int UpdateRow(CListCtrl *, int);
	void AppendForeignKeyFilter(CString &strFilter, LPCTSTR lpTablePK, LPCTSTR lpColumnPK, LPCTSTR lpColumnFK);
// Generierte Message-Map-Funktionen
protected:
	//{{AFX_MSG(CChildFrame)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	afx_msg void OnDestroy();
	afx_msg void OnParentNotify(UINT message, LPARAM lParam);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnQueryNew();
	afx_msg void OnEnterIdle(UINT nWhy, CWnd* pWho);
	afx_msg LRESULT OnMenuChar(UINT nChar, UINT nFlags, CMenu* pMenu);
	afx_msg void OnFileOpen();
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditClear(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditSelectAll(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnEditClear();
	afx_msg void OnEditCopy();
	afx_msg void OnEditCut();
	afx_msg void OnEditPaste();
	afx_msg void OnEditSelectAll();
	afx_msg void OnEditRedo();
	afx_msg void OnEditUndo();
	afx_msg void OnEditCopyHtml();
	afx_msg void OnZoomIn();
	afx_msg void OnZoomOut();
	afx_msg void OnZoomZero();
	afx_msg void OnUpdateZoom(CCmdUI *pCmdUI);
	afx_msg void OnUpdateZoomZero(CCmdUI *pCmdUI);
	afx_msg void OnUpdateOvrIndicator(CCmdUI *pCmdUI);
	//}}AFX_MSG
	afx_msg void OnCustomdrawRangeLv(UINT, NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDBRowDeleteitemRangeLv(UINT, NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDBRowGetdispinfoRangeLv(UINT, NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDBRowSortcolumnRangeLv(UINT, NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDBRowKeydownRangeLv(UINT, NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDBRowSetfocusRangeLv(UINT, NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDBRowItemchangingRangeLv(UINT, NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDBRowItemchangedRangeLv(UINT, NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDBRowActivateRangeLv(UINT, NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDBRowClickRangeLv(UINT, NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDBRowEditSetfocus(UINT);
	afx_msg void OnDBRowEditKillfocus(UINT);
	afx_msg void OnSelchangeRangeTc(UINT, NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSortcolumnRangeLv(UINT, NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnKeydownRangeLv(UINT, NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDblclkLvTables(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDblclkLvProcedures(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSelchangeCbUse();
	afx_msg void OnDropdownTb(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnIdleUpdateCmdUI();
	DECLARE_MESSAGE_MAP()
	afx_msg LPDISPATCH rdoConnxn();
	afx_msg LPDISPATCH rdoEngine();
	afx_msg LPDISPATCH ShowConsole(long nCmdShow);
	afx_msg BOOL FreeConsole();
	afx_msg void SetLogStyle(long style, LPCTSTR pszStyle);
	afx_msg BSTR GetLogStyle(long style);
	DECLARE_DISPATCH_MAP()
// Event reflectors
	afx_msg void OnStatusTextChange(LPCTSTR pszText);
	DECLARE_EVENTSINK_MAP()

	DECLARE_OLECMD_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.
