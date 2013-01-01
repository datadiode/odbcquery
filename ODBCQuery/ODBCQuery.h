class CODBCQueryDoc;

class CODBCQueryDocTemplate
: public CMultiDocTemplate
, public CRecentFileList
{
private:
	static const BYTE CODBCQueryDocTemplate::rgLocalPassword[];
	static void RemoveAttribute(CString &, LPCTSTR);
public:
	enum
	{
		iImagePK = 0,
		iImageFilter = 1,
	};
	CMenu m_menuContext;
	CImageList m_imagelist;
	CODBCQueryDocTemplate(
		UINT nIDResource,
		CRuntimeClass *pDocClass,
		CRuntimeClass *pFrameClass,
		CRuntimeClass *pViewClass,
		UINT nStart, LPCTSTR lpszSection,
		LPCTSTR lpszEntryFormat, int nSize,
		int nMaxDispLen = AFX_ABBREV_FILENAME_LEN);
	~CODBCQueryDocTemplate();
	int GetDocumentCount() const { return m_docList.GetCount(); }
	CODBCQueryDoc *Connect(LPCTSTR lpszConnectString, LPCTSTR lpszFileName, LPCTSTR lpszInitialCatalog, int nMakeVisible = 1);
	virtual CDocument *OpenDocumentFile(LPCTSTR lpszPathName, BOOL bMakeVisible = TRUE);
	virtual void InitialUpdateFrame(CFrameWnd* pFrame, CDocument* pDoc,
		BOOL bMakeVisible = TRUE);
	virtual void Add(LPCTSTR);
	virtual void UpdateMenu(CCmdUI *);
	CODBCQueryDoc *FindDocument(LPCTSTR) const;
	void ParseCommandLine(LPTSTR);
};

class CODBCQueryApp : public CWinApp
{
public:
	using CWinApp::m_pRecentFileList;
	CODBCQueryApp();
	CODBCQueryDocTemplate *m_pDocTemplate;
	//{{AFX_VIRTUAL(CODBCQueryApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual CDocument* OpenDocumentFile(LPCTSTR lpszFileName); // open named file
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CODBCQueryApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	afx_msg void OnUpdateRecentConnMenu(CCmdUI* pCmdUI);
	afx_msg BOOL OnOpenRecentConn(UINT nID);
	DECLARE_MESSAGE_MAP()
};

extern CODBCQueryApp theApp;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.
