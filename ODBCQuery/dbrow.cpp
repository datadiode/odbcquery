/*/DBRow.cpp

Last edit: 2013-01-05 Jochen Neubeck

[The MIT license]

Copyright (c) 2007 Jochen Neubeck

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include "stdafx.h"
#include "DBRow.h"

#pragma comment(lib, "Rpcrt4.lib")

#define LOCALE_DECIMAL \
	(MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT))

CDBRow::Item::Item(const signed char *data, size_t size)
: data(data), size(size)
{
}

class CDBRow::Handle::Scan
{
private:
	CRecordset &stmt;
	const short count;
	int index;
	size_t size;
	void Recurse();
public:
	Handle *handle;
	Scan(CRecordset &stmt)
	: handle(0)
	, stmt(stmt)
	, count(stmt.GetODBCFieldCount())
	, index(0)
	, size((count + 1) * sizeof(size_t))
	{
		Recurse();
	}
};

void CDBRow::Handle::Scan::Recurse()
{
	char *ptr = 0;
	if (index < count)
	{
		CDBVariant var;
		short nSQLType = stmt.m_rgODBCFieldInfos[index].m_nSQLType;
		switch (nSQLType)
		{
		case SQL_LONGVARCHAR:
		case SQL_WLONGVARCHAR:
		case SQL_WCHAR:
		case SQL_WVARCHAR:
			stmt.m_rgODBCFieldInfos[index].m_nSQLType = SQL_LONGVARBINARY;
			break;
		case SQL_GUID:
			stmt.m_rgODBCFieldInfos[index].m_nSQLType = SQL_VARCHAR;
			break;
		}
		stmt.GetFieldValue(index, var);
		stmt.m_rgODBCFieldInfos[index].m_nSQLType = nSQLType;
		const void *pvData = &var.m_dblVal;
		size_t cbData = 0;
		signed char vt = 0;
		switch (var.m_dwType)
		{
		case DBVT_NULL:
			vt = VT_NULL;
			break;
		case DBVT_BOOL:
			vt = VT_BOOL;
			cbData = sizeof(char);
			break;
		case DBVT_UCHAR:
			vt = VT_UI1;
			cbData = sizeof(char);
			break;
		case DBVT_SHORT:
			vt = VT_I2;
			cbData = sizeof(short);
			break;
		case DBVT_LONG:
			vt = VT_I4;
			cbData = sizeof(long);
			break;
		case DBVT_SINGLE:
			vt = VT_R4;
			cbData = sizeof(float);
			break;
		case DBVT_DOUBLE:
			vt = VT_R8;
			cbData = sizeof(double);
			break;
		case DBVT_DATE:
			vt = VT_DATE;
			pvData = var.m_pdate;
			cbData = sizeof(TIMESTAMP_STRUCT);
			break;
		case DBVT_STRING:
#if _MFC_VER > 0x0700
		case DBVT_ASTRING:
		case DBVT_WSTRING:
#endif
			switch (nSQLType)
			{
			case SQL_NUMERIC:
			case SQL_DECIMAL:
			case SQL_GUID:
				if (BSTR bstrVal = var.m_pstring->AllocSysString())
				{
					C_ASSERT(sizeof(TIMESTAMP_STRUCT) == sizeof(DECIMAL));
					C_ASSERT(sizeof(TIMESTAMP_STRUCT) == sizeof(GUID));
					pvData = new TIMESTAMP_STRUCT;
					cbData = sizeof(TIMESTAMP_STRUCT);
					if (nSQLType == SQL_GUID)
					{
						if (::UuidFromString((unsigned char *)&**var.m_pstring, (GUID *)pvData) == RPC_S_OK)
							vt = VT_CLSID;
					}
					else
					{
						if (SUCCEEDED(::VarDecFromStr(bstrVal, LOCALE_DECIMAL, 0, (DECIMAL *)pvData)))
							vt = VT_DECIMAL;
					}
					::SysFreeString(bstrVal);
					if (vt)
					{
						delete var.m_pstring;
						var.m_pdate = (TIMESTAMP_STRUCT *)pvData;
						var.m_dwType = DBVT_DATE;
						break;
					}
					delete (TIMESTAMP_STRUCT *)pvData;
				}
				// fall through
			case SQL_CHAR:
				var.m_pstring->TrimRight(VK_SPACE);
				// fall through
			default:
				vt = VT_BSTR;
				pvData = var.m_pstring->operator LPCTSTR();
				cbData = var.m_pstring->GetLength() * sizeof(TCHAR);
				break;
			}
			break;
		case DBVT_BINARY:
			vt = VT_BLOB;
			cbData = var.m_pbinary->m_dwDataLength;
			switch (nSQLType)
			{
			case SQL_WCHAR:
			case SQL_WVARCHAR:
			case SQL_WLONGVARCHAR:
				cbData &= ~1;
				// fall through
			case SQL_LONGVARCHAR:
			case SQL_BINARY:
			case SQL_VARBINARY:
			case SQL_LONGVARBINARY:
				vt = static_cast<signed char>(nSQLType);
			}
			pvData = GlobalLock(var.m_pbinary->m_hData);
			break;
		}
		++index;
		size += 1 + cbData;
		Recurse();
		--index;
		memcpy(handle->data + (size -= cbData), pvData, cbData);
		handle->data[--size] = vt;
	}
	else
	{
		handle = (Handle *)CObject::operator new(sizeof(CObject) + size);
		handle->Handle::Handle();
	}
	handle->offset[index] = size;
};

CDBRow::Handle *CDBRow::Scan(CRecordset &stmt)
{
	return Handle::Scan(stmt).handle;
}

CString CDBRow::Handle::asString(LPCTSTR sep) const
{
	if (sep == NULL)
		sep = _T(", ");
	CString s;
	for (size_t i = 0 ; i < Count() ; ++i)
	{
		if (i)
		{
			s += sep;
		}
		s += Item(i)->asString();
	}
	return s;
}

CString CDBRow::Handle::asQuoted(LPCTSTR sep) const
{
	if (sep == NULL)
		sep = _T(", ");
	CString s;
	for (size_t i = 0 ; i < Count() ; ++i)
	{
		if (i)
		{
			s += sep;
		}
		s += Item(i)->asString(_T("#ERR"), CDBRow::Item::fEnquoted);
	}
	return s;
}

static LPTSTR HexDump(LPTSTR pch, LPBYTE pb, DWORD cb)
{
	while (cb)
	{
		pch += wsprintf(pch, _T("%02X"), (UINT)*pb++);
		--cb;
	}
	return pch;
}

template<class T>
CString EnquoteString(const T *data, size_t cb, size_t quot, size_t limit = 120)
{
	size_t innerquots = 0;
	size_t dots = 0;
	if (cb > limit)
	{
		cb = limit;
		dots = 3;
	}
	if (quot)
	{
		for (size_t i = 0 ; i < cb ; ++i)
		{
			if (data[i] == '\'')
				++innerquots;
		}
	}
	CString s;
	// NB: Unicode needs an axtra byte for proper termination
	T *pch = (T *)s.GetBufferSetLength(
		(quot + cb + innerquots + dots + quot) * sizeof(T) + sizeof(T) - 1);
	if (quot)
		*pch++ = '\'';
	size_t i;
	for (i = 0 ; i < cb ; ++i)
	{
		switch (T c = data[i])
		{
		case '\'':
			if (quot)
				*pch++ = c;
			// fall through
		default:
			*pch++ = c;
		}
	}
	for (i = 0 ; i < dots ; ++i)
		*pch++ = '.';
	if (quot)
		*pch++ = '\'';
	*pch++ = '\0'; // extra byte for Unicode termination as mentioned above
	return s;
}

CString CDBRow::Item::asString(LPCTSTR null, UINT flags) const
{
	TCHAR buffer[260];
	union
	{
		BOOL				m_boolVal;
		unsigned char		m_chVal;
		short				m_iVal;
		long				m_lVal;
		float				m_fltVal;
		double				m_dblVal;
		TIMESTAMP_STRUCT	m_date;
		DECIMAL				m_decVal;
		GUID				m_guid;
	} u;
	BSTR bstrVal;
	LPCTSTR fmt = flags & fEnquoted ? null : _T("?");
	size_t cb = 0;
	memset(&u, 0, sizeof u);
	switch (*data)
	{
	case VT_NULL:
		return flags & fEnquoted ? "NULL" : null;
	case SQL_LONGVARCHAR:
	case VT_BSTR:
		return EnquoteString(data + 1, size - 1, flags & fEnquoted ? 1 : 0);
	case SQL_WLONGVARCHAR:
	case SQL_WCHAR:
	case SQL_WVARCHAR:
		ASSERT(((size - 1) & 1) == 0);
		return EnquoteString(
			LPCWSTR(data + 1), (size - 1) / 2, flags & fEnquoted ? 1 : 0);
	case VT_UI1:
		cb = sizeof(unsigned char);
		fmt = _T("%u");
		break;
	case VT_BOOL:
		cb = sizeof(char);
		fmt = _T("%d");
		break;
	case VT_I2:
		cb = sizeof(short);
		fmt = _T("%hd");
		break;
	case VT_I4:
		cb = sizeof(long);
		fmt = _T("%ld");
		break;
	case VT_R4:
		memcpy(&u, data + 1, sizeof(float));
		u.m_dblVal = u.m_fltVal;
		fmt = _T("%g");
		break;
	case VT_R8:
		cb = sizeof(double);
		fmt = _T("%g");
		break;
	case VT_DATE:
		memcpy(&u, data + 1, sizeof(TIMESTAMP_STRUCT));
		_stprintf(buffer,
			flags & fEnquoted ?
			_T("{ts '%04d-%02d-%02d %02d:%02d:%02d.%03d'}") :
			_T("%04d-%02d-%02d %02d:%02d:%02d.%03d"),
			(int)u.m_date.year, (int)u.m_date.month, (int)u.m_date.day,
			(int)u.m_date.hour, (int)u.m_date.minute, (int)u.m_date.second,
			(int)u.m_date.fraction / 1000000);
		return buffer;
	case SQL_LONGVARBINARY:
	case SQL_BINARY:
	case SQL_VARBINARY:
	case VT_BLOB:
		{
			cb = size - 1;
			size_t dots = 0;
			size_t quot = flags & fEnquoted ? 1 : 0;
			if ((flags & fTruncate) && cb > 120)
			{
				cb = 120;
				dots = 3;
			}
			size_t cb2 = 2 * cb;
			CString s;
			LPTSTR pch = s.GetBufferSetLength(quot + cb2 + dots + quot);
			HexDump(pch + quot, (BYTE *)data + 1, cb);
			memcpy(pch + quot + cb2, "...", dots);
			if (quot)
				pch[0] = pch[quot + cb2 + dots] = '\'';
			return s;
		}
	case VT_CLSID:
		memcpy(&u, data + 1, sizeof(GUID));
		_stprintf(buffer,
			flags & fEnquoted ?
			_T("{guid '%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x'}") :
			_T("%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x"),
			u.m_guid.Data1, u.m_guid.Data2, u.m_guid.Data3,
			u.m_guid.Data4[0], u.m_guid.Data4[1], u.m_guid.Data4[2], u.m_guid.Data4[3],
			u.m_guid.Data4[4], u.m_guid.Data4[5], u.m_guid.Data4[6], u.m_guid.Data4[7]);
		return buffer;
	case VT_DECIMAL:
		memcpy(&u, data + 1, sizeof(DECIMAL));
		if (SUCCEEDED(::VarBstrFromDec(&u.m_decVal, LOCALE_DECIMAL, 0, &bstrVal)))
		{
			CString s(bstrVal);
			::SysFreeString(bstrVal);
			return s;
		}
		break;
	}
	_vstprintf(buffer, fmt, (va_list)memcpy(&u, data + 1, cb));
	return buffer;
}

const CString &CDBRow::Enquote(CString &s)
{
	static const TCHAR szQuotes[] = _T("''");
	s.Replace(szQuotes + 1, szQuotes);
	s.Insert(0, szQuotes + 1);
	s += szQuotes + 1;
	return s;
}

CString CDBRow::MakeString(BSTR bstr)
{
	int cb = ::SysStringLen(bstr);
	if (cb)
		cb = 2 * cb + 1;
	return CString((PSTR)bstr, cb);
}

void CDBRow::Item::GetRef(COPYDATASTRUCT &cds) const
{
	cds.dwData = BYTE(*data);
	cds.lpData = PVOID(data + 1);
	cds.cbData = size - 1;
}

int CDBRow::Item::Type() const
{
	return *data;
}

template<class T>
inline int Compare(T p, T q)
{
	return p > q ? 1 : p < q ? -1 : 0;
}

int CDBRow::Item::Compare(const Item &r) const
{
	if (int cmp = *data - *r.data)
		return cmp;
	union
	{
		BOOL				m_boolVal:8;
		unsigned char		m_chVal;
		short				m_iVal;
		long				m_lVal;
		float				m_fltVal;
		double				m_dblVal;
		TIMESTAMP_STRUCT	m_date;
		DECIMAL				m_decVal;
		GUID				m_guid;
	} u, v;
	switch (*data)
	{
	case VT_NULL:
		return 0;
	case VT_BSTR:
		if (int cmp = StrCmpNI(LPCSTR(data + 1), LPCSTR(r.data + 1), (size <= r.size ? size : r.size) - 1))
			return cmp;
		return size - r.size;
	case SQL_WLONGVARCHAR:
	case SQL_WCHAR:
	case SQL_WVARCHAR:
		if (int cmp = StrCmpNIW(LPCWSTR(data + 1), LPCWSTR(r.data + 1), (size <= r.size ? size : r.size) / 2 - 1))
			return cmp;
		return size - r.size;
	case VT_UI1:
		memcpy(&u, data + 1, sizeof(unsigned char));
		memcpy(&v, r.data + 1, sizeof(unsigned char));
		return ::Compare(u.m_chVal, v.m_chVal);
	case VT_BOOL:
		memcpy(&u, data + 1, sizeof(char));
		memcpy(&v, r.data + 1, sizeof(char));
		return ::Compare(u.m_boolVal, v.m_boolVal);
	case VT_I2:
		memcpy(&u, data + 1, sizeof(short));
		memcpy(&v, r.data + 1, sizeof(short));
		return ::Compare(u.m_iVal, v.m_iVal);
	case VT_I4:
		memcpy(&u, data + 1, sizeof(long));
		memcpy(&v, r.data + 1, sizeof(long));
		return ::Compare(u.m_lVal, v.m_lVal);
	case VT_R4:
		memcpy(&u, data + 1, sizeof(float));
		memcpy(&v, r.data + 1, sizeof(float));
		return ::Compare(u.m_fltVal, v.m_fltVal);
	case VT_R8:
		memcpy(&u, data + 1, sizeof(double));
		memcpy(&v, r.data + 1, sizeof(double));
		return ::Compare(u.m_dblVal, v.m_dblVal);
	case VT_DATE:
		memcpy(&u, data + 1, sizeof(TIMESTAMP_STRUCT));
		memcpy(&v, r.data + 1, sizeof(TIMESTAMP_STRUCT));
		if (u.m_date.year != v.m_date.year)
			return u.m_date.year > v.m_date.year ? 1 : -1;
		if (u.m_date.month != v.m_date.month)
			return u.m_date.month > v.m_date.month ? 1 : -1;
		if (u.m_date.day != v.m_date.day)
			return u.m_date.day > v.m_date.day ? 1 : -1;
		if (u.m_date.hour != v.m_date.hour)
			return u.m_date.hour > v.m_date.hour ? 1 : -1;
		if (u.m_date.minute != v.m_date.minute)
			return u.m_date.minute > v.m_date.minute ? 1 : -1;
		if (u.m_date.second != v.m_date.second)
			return u.m_date.second > v.m_date.second ? 1 : -1;
		if (u.m_date.fraction != v.m_date.fraction)
			return u.m_date.fraction > v.m_date.fraction ? 1 : -1;
		return 0;
	case VT_CLSID:
		memcpy(&u, data + 1, sizeof(GUID));
		memcpy(&v, r.data + 1, sizeof(GUID));
		if (u.m_guid.Data1 != v.m_guid.Data1)
			return u.m_guid.Data1 > v.m_guid.Data1 ? 1 : -1;
		if (u.m_guid.Data2 != v.m_guid.Data2)
			return u.m_guid.Data2 > v.m_guid.Data2 ? 1 : -1;
		if (u.m_guid.Data3 != v.m_guid.Data3)
			return u.m_guid.Data3 > v.m_guid.Data3 ? 1 : -1;
		return memcmp(u.m_guid.Data4, v.m_guid.Data4, 8);
	case VT_BLOB:
		if (int cmp = memcmp(data + 1, r.data + 1, (size <= r.size ? size : r.size) - 1))
			return cmp;
		return size - r.size;
	case VT_DECIMAL:
		memcpy(&u, data + 1, sizeof(DECIMAL));
		memcpy(&v, r.data + 1, sizeof(DECIMAL));
		return ::VarDecCmp(&u.m_decVal, &v.m_decVal) - VARCMP_EQ;
	}
	return 0;
}

int CALLBACK CDBRow::Handle::LVCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	Handle *pRow1 = reinterpret_cast<Handle *>(lParam1);
	Handle *pRow2 = reinterpret_cast<Handle *>(lParam2);
	if (pRow1 == 0)
		return 1;
	if (pRow2 == 0)
		return -1;
	int nCol = lParamSort >= 0 ? lParamSort : ~lParamSort;
	int cmp = pRow1->Item(nCol)->Compare(pRow2->Item(nCol));
	return lParamSort >= 0 ? cmp : -cmp;
}

void CDBRow::WriteString(CFile *const pFile, LPCSTR pchOctets)
{
	if (pFile)
	{
		size_t cchAhead = strlen(pchOctets);
		while (LPCSTR pchAhead = (LPCSTR)memchr(pchOctets, '\n', cchAhead))
		{
			int cchLine = pchAhead - pchOctets;
			pFile->Write(pchOctets, cchLine);
			static const char eol[] = { '\r', '\n' };
			pFile->Write(eol, sizeof eol);
			++cchLine;
			pchOctets += cchLine;
			cchAhead -= cchLine;
		}
		pFile->Write(pchOctets, cchAhead);
	}
}

void CDBRow::MakeHtmlEntities(CString &s)
{
	BSTR bstr = s.AllocSysString();
	for (int i = s.GetLength() ; i-- ; )
	{
		WCHAR c = bstr[i];
		TCHAR cTmp[14];
		LPCTSTR pszSubst = 0;
		switch (c)
		{
		case '<':
			pszSubst = _T("&lt");
			break;
		case '>':
			pszSubst = _T("&gt");
			break;
		case '&':
			pszSubst = _T("&amp");
			break;
		default:
			if (c & ~0x7F)
			{
				wsprintf(cTmp, _T("&#%u"), c);
				pszSubst = cTmp;
			}
			break;
		}
		if (pszSubst)
		{
			s.SetAt(i, ';');
			s.Insert(i, pszSubst);
		}
	}
	SysFreeString(bstr);
}

UINT CDBRow::WriteHtcf(CFile *pf, UINT htcf)
{
	static const char header[] =
		"Version:0.9\n"
		"StartHTML:%09d\n"
		"EndHTML:%09d\n"
		"StartFragment:%09d\n"
		"EndFragment:%09d\n";
	static const char start[] = "<html>\n<body>\n<!--StartFragment -->\n";
	static const char end[] = "\n<!--EndFragment -->\n</body>\n</html>\n";
	char buffer[256];
	if (htcf == 0)
	{
		// Write preliminary CF_HTML header with all offsets zero
		htcf = wsprintfA(buffer, header, 0, 0, 0, 0);
		pf->Write(buffer, htcf);
		pf->Write(start, sizeof start - 1);
	}
	else
	{
		pf->Write(end, sizeof end); // include terminating zero
		DWORD size = pf->GetLength();
		// Rewrite CF_HTML header with valid offsets
		pf->SeekToBegin();
		wsprintfA(buffer, header, htcf, size - 1,
			htcf + sizeof start - 1, size - sizeof end + 1);
		pf->Write(buffer, htcf);
		htcf = RegisterClipboardFormat(_T("HTML Format"));
	}
	return htcf;
}

UINT CDBRow::WriteReport(CListCtrl &Lv, CFile *pfText, CFile *pfHtml, UINT htcf)
{
	CHeaderCtrl *pHd = static_cast<CHeaderCtrl *>(CWnd::FromHandle(
		reinterpret_cast<HWND>(Lv.SendMessage(LVM_GETHEADER))));
	int nCols = pHd->GetItemCount();
	int iRow = 0;
	int cxChar = Lv.GetStringWidth(_T("0"));
	LVITEM item;
	item.iItem = -1;
	item.mask = LVIF_PARAM;
	LVCOLUMN lvc;
	TCHAR szText[MAX_PATH]; // Assuming max col header will never be > MAX_PATH
	lvc.mask = LVCF_TEXT | LVCF_WIDTH;
	lvc.pszText = szText;
	lvc.cchTextMax = MAX_PATH;
	//WriteString(pfText, _T("\xEF\xBB\xBF"));
	// Write preliminary CF_HTML header with all offsets zero
	if (htcf)
		htcf = WriteHtcf(pfHtml);
	// Use window text as title
	CString sTitle;
	Lv.GetWindowText(sTitle);
	CString sTsNow = COleDateTime::GetCurrentTime().Format();
	WriteString(pfText, sTitle);
	WriteString(pfText, _T("\n"));
	WriteString(pfText, sTsNow);
	WriteString(pfText, _T("\n"));
	WriteString(pfHtml, _T("<h3>"));
	WriteString(pfHtml, sTitle);
	WriteString(pfHtml, _T("</h3>\n<p>"));
	WriteString(pfHtml, sTsNow);
	WriteString(pfHtml, _T("</p>\n"));
	// Write table contents
	WriteString(pfHtml, _T("<table border='1'>\n"));
	item.iSubItem = 0;
	WriteString(pfHtml, _T("<tr><th>"));
	while (item.iSubItem < nCols)
	{
		// Write text
		if (Lv.GetColumn(item.iSubItem, &lvc))
		{
			CString s = lvc.pszText;
			WriteString(pfText, s);
			MakeHtmlEntities(s);
			WriteString(pfHtml, s);
		}
		// Advance to next column
		++item.iSubItem;
		// Write delimiter
		WriteString(pfText, item.iSubItem < nCols ? _T("\t") : _T("\n"));
		WriteString(pfHtml, item.iSubItem < nCols ? _T("</th><th>") : _T("</th></tr>\n"));
	}
	while ((item.iItem = Lv.GetNextItem(item.iItem, LVNI_SELECTED)) != -1)
	{
		++iRow;
		item.iSubItem = 0;
		Lv.GetItem(&item);
		Handle *pRow = (Handle *)item.lParam;
		if (pRow == 0)
			break;
		WriteString(pfHtml, _T("<tr><td>"));
		while (item.iSubItem < nCols)
		{
			// Write text
			CString s = pRow->Item(item.iSubItem)->asString();
			if (int n = s.GetLength())
			{
				if (s.GetAt(n - 1))
				{
					WriteString(pfText, s);
					// Looks like an ANSI string - convert to Unicode
					CString u;
					int cch = MultiByteToWideChar(CP_ACP, 0, s, n, 0, 0);
					LPWSTR pch = (LPWSTR)u.GetBufferSetLength(cch * sizeof(WCHAR) + 1);
					MultiByteToWideChar(CP_ACP, 0, s, n, pch, cch);
					pch[cch] = '\0';
					s = u;
				}
				else
				{
					WriteString(pfText, CString((LPCWSTR)(LPCSTR)s));
				}
				if (pfHtml)
				{
					// convert to UTF8
					n = s.GetLength() / sizeof(WCHAR);
					int cch = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)(LPCTSTR)s, n, 0, 0, 0, 0);
					CString u;
					LPTSTR pch = u.GetBufferSetLength(cch);
					WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)(LPCTSTR)s, n, pch, cch, 0, 0);
					//MakeHtmlEntities(s);
					WriteString(pfHtml, u);
				}
			}
			// Advance to next column
			++item.iSubItem;
			// Write delimiter
			WriteString(pfText, item.iSubItem < nCols ? _T("\t") : _T("\n"));
			WriteString(pfHtml, item.iSubItem < nCols ? _T("</td><td>") : _T("</td></tr>\n"));
		}
	}
	WriteString(pfHtml, _T("</table>\n"));
	if (htcf)
		htcf = WriteHtcf(pfHtml, htcf);
	return htcf;
}
