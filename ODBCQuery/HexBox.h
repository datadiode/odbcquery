// TextBox.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CHexBox 
class IHexEditorWindow;

class CHexBox : public CDialog
{
// Konstruktion
public:
	CHexBox(CWnd* pParent = NULL);   // Standardkonstruktor
	~CHexBox();
// Dialogfelddaten
	CListCtrl *m_pLvResults;
	COPYDATASTRUCT m_cds;
	CComBSTR m_text;
	//{{AFX_DATA(CHexBox)
	enum { IDD = IDD_HEXBOX };
	CWnd m_heksedit;
	//}}AFX_DATA
protected:

	CStatusBar	m_wndStatusBar;
	IHexEditorWindow *m_pif;

// �berschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktions�berschreibungen
	//{{AFX_VIRTUAL(CHexBox)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterst�tzung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CHexBox)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ f�gt unmittelbar vor der vorhergehenden Zeile zus�tzliche Deklarationen ein.
