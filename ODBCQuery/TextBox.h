// TextBox.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CTextBox 

class CTextBox : public CDialog
{
// Konstruktion
public:
	CTextBox(CWnd* pParent = NULL);   // Standardkonstruktor
	~CTextBox();
// Dialogfelddaten
	CListCtrl *m_pLvResults;
	COPYDATASTRUCT m_cds;
	CComBSTR m_text;
	//{{AFX_DATA(CTextBox)
	enum { IDD = IDD_TEXTBOX };
	CScintillaCtrl m_scintilla;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CTextBox)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementierung
protected:
	LRESULT SendEditor(UINT, WPARAM = 0, LPARAM = 0);
	void DefineMarker(int marker, int markerType, COLORREF fore, COLORREF back);
	void FoldChanged(int line, int levelNow, int levelPrev);
	void FoldChanged(int position);
	void Expand(int &line, bool doExpand, bool force = false,
	        int visLevels = 0, int level = -1);
	void FoldAll();
	void ToggleFoldRecursive(int line, int level);
	void EnsureAllChildrenVisible(int line, int level);
	bool MarginClick(int position, int modifiers);
	void Notify(SCNotification *notification);
	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CTextBox)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.
