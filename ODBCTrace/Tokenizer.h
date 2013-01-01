class CTokenizer
{
	CTokenizer(); // disallow construction
public:
	static const TCHAR szTrim[];
	TCHAR szText[1];
	typedef CTokenizer *From;
	CTokenizer *EatSpace();
	CTokenizer *EatKeyword(LPCTSTR, BOOL fCaseSens = FALSE);
	int LookAhead(int depth = 0);
};


C_ASSERT(sizeof(CTokenizer) == sizeof(TCHAR)); // essential for pointer arith
