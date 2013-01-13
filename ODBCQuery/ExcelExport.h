class CExcelExport
{
public:
	CExcelExport();
	~CExcelExport();
	operator HRESULT() { return hr; }
	bool Open(LPCWSTR);
	void Close(int nShow = SW_HIDE);
	void ApplyProfile(LPCTSTR app, LPCTSTR ini, bool fWriteDefaults = false);
	void WriteWorkbook(CListCtrl *);
	BOOL fPrintGrid : 1;
	CString sHeader;
	CString sFooter;
	CString sViewer;
	int nShowViewer;
	CStringA sSheetName;
private:
	struct BiffRecord;
	void WriteCellValue(int, int, WORD, LPCSTR, const CODBCFieldInfo &);
	HRESULT hr;
	IStorage *pstg;
	IStream *pstm;
};
