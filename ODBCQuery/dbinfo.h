
class CDBInfo
{
public:
	CDatabaseEx *const m_pDatabase;
	CFile *const m_pfText;
	CFile *const m_pfHtml;
	CDBInfo(CDatabaseEx *pDatabase, CFile *pfText, CFile *pfHtml)
	: m_pDatabase(pDatabase), m_pfText(pfText), m_pfHtml(pfHtml)
	{
	}
	void WriteDriverInfo();
	void WriteFunctions();
	void WriteSupportedSQL();
	void WriteDataTypes();
	void WriteIdentifiers();
	void WriteLimits();
	void WriteTransactions();
	void WriteMiscellaneous();
private:
	void BeginTable();
	void EndTable();
	void WriteTitle(LPCSTR);
	void WriteValue(LPCSTR, LPCSTR, int ichAlign = 25);
	void WriteValue(LPCTSTR, int, LPCTSTR, int ichAlign = 25);
	void WriteValue(LPCTSTR, LPCTSTR, LPCTSTR, int ichAlign = 25);
	void WriteValueAndVersion(LPCSTR, LPCSTR, LPCSTR, int ichAlign = 25);
	void WriteTableRow(LPCTSTR fmt, CString *ps);
	static void EnsureAnsi(CString &);
	static void EnsureAnsi(LPCTSTR fmt, CString *ps);
	static int LookAheadForDoubleSpace(LPCTSTR);
	void WriteTableHeader(LPCTSTR);
	int AppendValue(int ich, LPCTSTR szSqlType, int ichAlign = 20);
	void WriteKeywords(const char *);
	HDBC GetHDBC()
	{
		return m_pDatabase->m_hdbc;
	}
	LPCSTR SQLGetInfoString(UWORD fInfoType, LPSTR buffer = STRRET().cStr)
	{
		SWORD cbData;
		::SQLGetInfo(GetHDBC(), fInfoType, buffer, 200, &cbData);
		return buffer;
	}
	UWORD SQLGetInfoShort(UWORD fInfoType)
	{
		UWORD value;
		SWORD cbData;
		::SQLGetInfo(GetHDBC(), fInfoType, &value, 2, &cbData);
		return value;
	}
	UDWORD SQLGetInfoLong(UWORD fInfoType)
	{
		UDWORD value;
		SWORD cbData;
		::SQLGetInfo(GetHDBC(), fInfoType, &value, 4, &cbData);
		return value;
	}
};
