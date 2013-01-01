class StatementInfo : public LIST_ENTRY
{
public:
	SQLHSTMT StatementHandle;
	LPTSTR StatementText;
	UINT nParams;
	UINT nColumns;
	struct Param
	{
		SQLSMALLINT	InputOutputType;
		SQLSMALLINT	ValueType;
		SQLSMALLINT	ParameterType;
		SQLUINTEGER	ColumnSize;
		SQLSMALLINT	DecimalDigits;
		SQLPOINTER	ParameterValuePtr;
		SQLINTEGER	BufferLength;
		SQLINTEGER	*StrLen_or_IndPtr;
	} rgParams[1];
	static StatementInfo *pHead;
	static void Insert(StatementInfo *);
	static void Delete(StatementInfo *);
	static StatementInfo *FromHandle(SQLHSTMT, UINT = 0, UINT = 0);
	void SetStatementText(LPCSTR, int);
	void SetStatementText(LPCWSTR, int);
	Param *GetParam(UINT);
	Param *GetColumn(UINT);
	static STDMETHODIMP WriteLiteral(ISequentialStream *, const Param *);
	STDMETHODIMP WriteSQL(ISequentialStream *, BOOL bTerminate);
	static LPTSTR BuildSQL(SQLHSTMT);
	STDMETHODIMP DumpColumns(ISequentialStream *, SQLINTEGER iRow, BOOL bTerminate);
	static LPTSTR ColumnDump(SQLHSTMT, SQLUINTEGER nRows = 1);
	// Dump an unbound column
	static STDMETHODIMP DumpUnboundColumn(ISequentialStream *, Param *, SQLHSTMT, SQLSMALLINT, BOOL bTerminate);
	static LPTSTR UnboundColumnDump(SQLHSTMT, SQLSMALLINT, SQLSMALLINT TargetType, SQLPOINTER TargetValuePtr, SQLINTEGER BufferLength, SQLINTEGER *StrLen_or_IndPtr);
};
