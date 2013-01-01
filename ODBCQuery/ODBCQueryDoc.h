#include "DatabaseEx.h"
#include "RecordsetEx.h"
#include "LvPopulator.h"
#include "StyleDefinition.h"

class CODBCQueryDoc : public CScintillaDoc
{
	DECLARE_DYNCREATE(CODBCQueryDoc)
public:
	CDatabaseEx m_db;
	DWORD m_dwTicks;
	CString m_strCmdLine;
	static CScintillaView *NewScintillaView();
	CRecordsetEx *GetCurrentRecordset();
	virtual BOOL SaveModified();
	virtual void OnIdle();
	BOOL CancelQuery();
	~CODBCQueryDoc();
protected:
	CODBCQueryDoc();
	CListCtrl *CreateListCtrl(CRecordsetEx *, const RECT &, CWnd *, UINT);
	//{{AFX_MSG(CODBCQueryDoc)};
	afx_msg void OnQueryExecute();
	afx_msg void OnUpdateQueryExecute(CCmdUI* pCmdUI);
	//}}AFX_MSG
	//afx_msg void OnHelpOdbcInfo();
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.
