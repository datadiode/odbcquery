//////////////////////////////////////////////////////////////////////
// CHelpers

class CHelpers
{
public:
	static BOOL Open(CFile &, LPCTSTR lpszFileName, UINT nOpenFlags);
	static LPTSTR AllocDispinfoText(const CString &);
private:
	static struct rgDispinfoText
	{
		CString item_0;
		CString item_1;
		operator CString *() { return &item_0; }
	} rgDispinfoText;
};

//////////////////////////////////////////////////////////////////////
// CSortListCtrl

class CSortListCtrl : public CListCtrl
{
public:
	using CListCtrl::m_nModalResult;
	static CSortListCtrl *From(NMHDR *);
	BOOL Sort(PFNLVCOMPARE);
	BOOL Sort();
private:
	CSortListCtrl(); // disallow construction
	static int CALLBACK Compare(LPARAM, LPARAM, LPARAM);
};

//////////////////////////////////////////////////////////////////////
// CDumpStream

template<TCHAR delimiter>
class CDumpStream
{
	CDumpStream(); // disallow construction
public:
	TCHAR szText[1];
	static CDumpStream *From(LPTSTR pszText)
	{
		pszText[0] = '\0';
		return (CDumpStream *)pszText;
	}
	CDumpStream *Advance()
	{
		CDumpStream *p = this + lstrlen(szText);
		p->szText[0] = delimiter;
		p->szText[1] = '\0';
		return p;
	}
	template<class T>
	CDumpStream *Value(T value)
	{
		wsprintf(szText + lstrlen(szText), _T("%d"), (int)value);
		return Advance();
	}
	CDumpStream *Value(LPCTSTR pszText, int cchTextMex)
	{
		lstrcpyn(szText + lstrlen(szText), pszText, cchTextMex);
		return Advance();
	}
};

C_ASSERT(sizeof(CDumpStream<';'>) == sizeof(TCHAR)); // essential for pointer arith

//////////////////////////////////////////////////////////////////////
// CScanStream

template<TCHAR delimiter>
class CScanStream
{
	CScanStream(); // disallow construction
public:
	TCHAR szText[1];
	static CScanStream *From(LPCTSTR pszText)
	{
		return (CScanStream *)pszText;
	}
	CScanStream *Advance()
	{
		if (this)
			if (LPCTSTR psz = StrChr(szText, delimiter))
				return this + (psz - szText) + 1;
		return 0;
	}
	template<class T>
	CScanStream *Value(T &value)
	{
		int iRet;
		if (this && StrToIntEx(szText, STIF_SUPPORT_HEX, &iRet))
			value = (T)iRet;
		return Advance();
	}
	CScanStream *Value(LPTSTR pszText, int cchTextMex)
	{
		CScanStream *p = Advance();
		if (p && cchTextMex > p - this)
			cchTextMex = p - this;
		if (this)
			lstrcpyn(pszText, szText, cchTextMex);
		return p;
	}
};

C_ASSERT(sizeof(CScanStream<';'>) == sizeof(TCHAR)); // essential for pointer arith

//////////////////////////////////////////////////////////////////////
// CLogFont

class CLogFont : public LOGFONT
{
public:
	void Clear() { memset(this, 0, sizeof(*this)); }
	template<class T>
	T *Stream(T *Stream)
	{
		return Stream
		-> Value(lfHeight)
		-> Value(lfWidth)
		-> Value(lfEscapement)
		-> Value(lfOrientation)
		-> Value(lfWeight)
		-> Value(lfItalic)
		-> Value(lfUnderline)
		-> Value(lfStrikeOut)
		-> Value(lfCharSet)
		-> Value(lfOutPrecision)
		-> Value(lfClipPrecision)
		-> Value(lfQuality)
		-> Value(lfPitchAndFamily)
		-> Value(lfFaceName, LF_FACESIZE);
	}
};
