/////////////////////////////////////////////////////////////////////////////
// CRecordsetEx
class CRecordsetEx : public CRecordset
{
	CRecordsetEx(); // disallow construction
public:
	enum
	{
		lOpenReadPrimaryKey = 10001,
		lOpenReadBestRowId = 10002,
		lOpenView = 10003,
	};
	enum
	{
		nNullabilityPK = 0x1000,
		nNullabilityAutoIncrement = 0x2000,
		nNullabilityUpdatable = 0x4000
	};
	static CRecordsetEx *From(CRecordset *prs)
	{
		return static_cast<CRecordsetEx *>(prs);
	}
	static CRecordsetEx *New(CDatabaseEx *);
	static int LookAheadSQL(LPCTSTR, TCHAR = _T(';'));
	static LPTSTR LookAheadTerminateSQL(LPTSTR);
	static int IsPrefix(const CString &, LPCTSTR);
	enum ExecuteStatus
	{
		pending,
		initial,
		initial_sp_helptext,
		initial_use,
		initial_sql,
		initial_catalog,
		skipped
	} Execute(int = forwardOnly);
	BOOL MoreResults();
	void ReplaceBrackets(CString &);
	void Release();
	using CRecordset::m_strSQL;
	using CRecordset::m_strTableName;
	using CRecordset::m_lOpen;
	using CRecordset::m_lCurrentRecord;
	using CRecordset::m_lRecordCount;
	using CRecordset::m_bUpdatable;
private:
	void BuildSelect(SQLCHAR *, SQLCHAR *);
};
