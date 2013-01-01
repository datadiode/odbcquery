// GenSQLDlg.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CGenSQLDlg 

class CGenSQLDlg : public CDialog
{
// Konstruktion
public:
	CGenSQLDlg(CDatabaseEx *pDatabase, CListCtrl *pLvResults);
// Dialogfelddaten
	//{{AFX_DATA(CGenSQLDlg)
	enum { IDD = IDD_GENERATE_SQL };
	CListCtrl	m_LvColumns;
	//}}AFX_DATA


// �berschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktions�berschreibungen
	//{{AFX_VIRTUAL(CGenSQLDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterst�tzung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	CDatabaseEx *const m_pDatabase;
	CListCtrl *const m_pLvResults;

	void OnContextMenuLvColumns(CPoint);
	void WriteFile(CFile &);

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CGenSQLDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ f�gt unmittelbar vor der vorhergehenden Zeile zus�tzliche Deklarationen ein.
