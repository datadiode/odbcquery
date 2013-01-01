/*/DBInfo.cpp

Last edit: 2013-01-01 Jochen Neubeck

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

// Based on knowledge distilled from the ODBCINFO MFC database sample.

#include "stdafx.h"
#include "DatabaseEx.h"
#include "RecordsetEx.h"
#include "DBRow.h"
#include "DBInfo.h"

static const TCHAR szSupported[] = _T("Supported");
static const TCHAR szNotSupported[] = _T("Not Supported");

void CDBInfo::WriteTitle(LPCTSTR pszText)
{
	CDBRow::WriteString(m_pfText, pszText);
	CDBRow::WriteString(m_pfText, _T("\n"));
	CDBRow::WriteString(m_pfText, CString('-', lstrlen(pszText)));
	CDBRow::WriteString(m_pfText, _T("\n"));
	CDBRow::WriteString(m_pfHtml, _T("<h3>"));
	CDBRow::WriteString(m_pfHtml, pszText);
	CDBRow::WriteString(m_pfHtml, _T("</h3>\n"));
}

void CDBInfo::BeginTable()
{
	CDBRow::WriteString(m_pfHtml, _T("<table border='1'>\n"));
}

void CDBInfo::EndTable()
{
	CDBRow::WriteString(m_pfText, _T("\n"));
	CDBRow::WriteString(m_pfHtml, _T("</table>\n"));
}

void CDBInfo::WriteValue(LPCTSTR pszLabel, LPCTSTR pszValue, int ichAlign)
{
	CString string;
	string.Format(_T("%*s - %s\n"), ichAlign, pszLabel, pszValue);
	CDBRow::WriteString(m_pfText, string);
	CString strValue = pszValue;
	CDBRow::MakeHtmlEntities(strValue);
	string.Format(_T("<tr><td>%s</td><td>%s</td></tr>\n"),
		pszLabel, strValue);
	CDBRow::WriteString(m_pfHtml, string);
}

void CDBInfo::WriteValue(LPCTSTR pszLabel, int iValue, LPCTSTR pszLookup, int ichAlign)
{
	CString strValue;
	if (!AfxExtractSubString(strValue, pszLookup, iValue, '\\'))
		strValue.Format(_T("%d"), iValue);
	WriteValue(pszLabel, strValue, ichAlign);
}

void CDBInfo::WriteValue(LPCTSTR pszLabel, LPCTSTR pszValue, LPCTSTR pszLookup, int ichAlign)
{
	int iValue = StrCSpn(pszLookup, pszValue) + 1;
	CString strValue;
	if (!AfxExtractSubString(strValue, pszLookup, iValue, '\\'))
		strValue = pszValue;
	WriteValue(pszLabel, strValue, ichAlign);
}

void CDBInfo::WriteValueAndVersion(LPCTSTR pszLabel, LPCTSTR pszValue,
	LPCTSTR pszVersion, int ichAlign)
{
	CString string;
	string.Format(_T("%*s - %s version %s\n"),
		ichAlign, pszLabel, pszValue, pszVersion);
	CDBRow::WriteString(m_pfText, string);
	string.Format(_T("<tr><td>%s</td><td>%s version %s</td></tr>\n"),
		pszLabel, pszValue, pszVersion);
	CDBRow::WriteString(m_pfHtml, string);
}

void CDBInfo::EnsureAnsi(CString &s)
{
	if (int n = s.GetLength())
		if (s.GetAt(n - 1) == '\0')
			s = CString((LPCWSTR)(LPCSTR)s, n / 2);
}

void CDBInfo::EnsureAnsi(LPCTSTR fmt, CString *ps)
{
	while ((fmt = StrChr(fmt, '%')) != 0)
	{
		EnsureAnsi(*ps);
		++fmt;
		++ps;
	}
}

void CDBInfo::WriteTableRow(LPCTSTR fmt, CString *ps)
{
	EnsureAnsi(fmt, ps);
	CString str;
	str.FormatV(fmt, (va_list)ps);
	CDBRow::WriteString(m_pfText, str);
	CDBRow::WriteString(m_pfHtml, _T("<tr>"));
	while ((fmt = StrChr(fmt, '%')) != 0)
	{
		CDBRow::MakeHtmlEntities(*ps);
		str.FormatV(_T("<td>%s</td>"), (va_list)ps);
		CDBRow::WriteString(m_pfHtml, str);
		++fmt;
		++ps;
	}
	CDBRow::WriteString(m_pfHtml, _T("</tr>\n"));
}

int CDBInfo::LookAheadForDoubleSpace(LPCTSTR psz)
{
	LPCTSTR pch = StrStr(psz, _T("  "));
	return pch ? pch - psz : lstrlen(psz);
}

void CDBInfo::WriteTableHeader(LPCTSTR pszText)
{
	CDBRow::WriteString(m_pfText, pszText);
	CDBRow::WriteString(m_pfText, _T("\n"));
	CDBRow::WriteString(m_pfHtml, _T("<tr>"));
	LPCTSTR pch = pszText;
	while (int cch = LookAheadForDoubleSpace(pch))
	{
		CDBRow::WriteString(m_pfText, CString('-', cch));
		CString str(pch, cch);
		CDBRow::MakeHtmlEntities(str);
		CDBRow::WriteString(m_pfHtml, _T("<th>"));
		CDBRow::WriteString(m_pfHtml, str);
		CDBRow::WriteString(m_pfHtml, _T("</th>"));
		pch += cch;
		cch = StrSpn(pch, _T(" "));
		CDBRow::WriteString(m_pfText, CString(' ', cch));
		pch += cch;
	}
	CDBRow::WriteString(m_pfText, _T("\n"));
	CDBRow::WriteString(m_pfHtml, _T("</tr>\n"));
}

int CDBInfo::AppendValue(int ich, LPCTSTR szSqlType, int ichAlign)
{
	LPCTSTR pszFmt = _T("\n                    %s");
	if (ich)
	{
		CDBRow::WriteString(m_pfText, _T(","));
		CDBRow::WriteString(m_pfHtml, _T(","));
		++ich;
	}
	else
	{
		ich = ichAlign;
	}
	int cch = lstrlen(szSqlType);
	if (ich + cch > 75)
	{
		CDBRow::WriteString(m_pfText, _T("\n"));
		CDBRow::WriteString(m_pfText, CString(' ', ichAlign));
		CDBRow::WriteString(m_pfHtml, _T("<br>"));
		ich = ichAlign;
	}
	CDBRow::WriteString(m_pfText, szSqlType);
	CDBRow::WriteString(m_pfHtml, szSqlType);
	return ich + cch;
}

void CDBInfo::WriteKeywords(const char *pch)
{
	CDBRow::WriteString(m_pfHtml, _T("<p>\n"));
	int ich = 0;
	while (int cch = StrCSpnA(pch += StrSpnA(pch, ","), ","))
	{
		if (pch[cch])
			++cch;
		if (ich + cch > 75)
		{
			CDBRow::WriteString(m_pfText, _T("\n"));
			CDBRow::WriteString(m_pfHtml, _T("<br>\n"));
			ich = 0;
		}
		CString str(pch, cch);
		CDBRow::WriteString(m_pfText, str);
		CDBRow::MakeHtmlEntities(str);
		CDBRow::WriteString(m_pfHtml, str);
		ich += cch;
		pch += cch;
	}
	CDBRow::WriteString(m_pfText, _T("\n\n"));
	CDBRow::WriteString(m_pfHtml, _T("</p>\n"));
}

void CDBInfo::WriteDriverInfo()
{
	WriteTitle(_T("Driver Information"));
	BeginTable();
	WriteValue(_T("Data Source Name"),		SQLGetInfoString(SQL_DATA_SOURCE_NAME));
	WriteValue(_T("Database Name"),			SQLGetInfoString(SQL_DATABASE_NAME));
	WriteValue(_T("User Name"),				SQLGetInfoString(SQL_USER_NAME));
	WriteValue(_T("Server Name"),			SQLGetInfoString(SQL_SERVER_NAME));
	WriteValueAndVersion(_T("DBMS"),		SQLGetInfoString(SQL_DBMS_NAME),
											SQLGetInfoString(SQL_DBMS_VER));
	WriteValueAndVersion(_T("Driver"),		SQLGetInfoString(SQL_DRIVER_NAME),
											SQLGetInfoString(SQL_DRIVER_VER));
	WriteValue(_T("Driver ODBC Ver."),		SQLGetInfoString(SQL_DRIVER_ODBC_VER));
	WriteValue(_T("ODBC Version"),			SQLGetInfoString(SQL_ODBC_VER));
	WriteValue(_T("API Conformance"),		SQLGetInfoShort(SQL_ODBC_API_CONFORMANCE),
											_T("None\\Level 1\\Level 2"));
	WriteValue(_T("SQL Conformance"),		SQLGetInfoShort(SQL_ODBC_SQL_CONFORMANCE),
											_T("Minimum\\Core\\Extended"));
	WriteValue(_T("SAG CLI Conformance"),	SQLGetInfoShort(SQL_ODBC_SAG_CLI_CONFORMANCE),
											_T("No\\Yes"));
	WriteValue(_T("Optional SQL IEF"),		SQLGetInfoString(SQL_ODBC_SQL_OPT_IEF),
											_T("NY\\Not Supported\\Supported"));
	WriteValue(_T("Active Connections"),	SQLGetInfoShort(SQL_ACTIVE_CONNECTIONS),
											_T("No Limit"));
	WriteValue(_T("Active Statements"),		SQLGetInfoShort(SQL_ACTIVE_STATEMENTS),
											_T("No Limit"));
	WriteValue(_T("Multiple Result Sets"),	SQLGetInfoString(SQL_MULT_RESULT_SETS),
											_T("NY\\Not Supported\\Supported"));
	WriteValue(_T("File Usage"),			SQLGetInfoShort(SQL_FILE_USAGE),
											_T("<n/a>, Driver not single-tier\\")
											_T("Files are Tables\\Files are Catalogs"));
	WriteValue(_T("Read/Write Status"),		SQLGetInfoString(SQL_DATA_SOURCE_READ_ONLY),
											_T("NY\\Data source is read-write\\")
											_T("Data source is read-only"));
	WriteValue(_T("Accessible Procedures"),	SQLGetInfoString(SQL_ACCESSIBLE_PROCEDURES),
											_T("NY\\No\\Yes"));
	WriteValue(_T("Accessible Tables"),		SQLGetInfoString(SQL_ACCESSIBLE_TABLES),
											_T("NY\\No\\Yes"));
	EndTable();
}

void CDBInfo::WriteFunctions()
{
	static const LPCTSTR rgFunctions[] =
	{
		0,
		_T("SQLAllocConnect"),
		_T("SQLAllocEnv"),
		_T("SQLAllocStmt"),
		_T("SQLBindCol"),
		_T("SQLCancel"),
		_T("SQLColAttributes"),
		_T("SQLConnect"),
		_T("SQLDescribeCol"),
		_T("SQLDisconnect"),
		_T("SQLError"),
		_T("SQLExecDirect"),
		_T("SQLExecute"),
		_T("SQLFetch"),
		_T("SQLFreeConnect"),
		_T("SQLFreeEnv"),
		_T("SQLFreeStmt"),
		_T("SQLGetCursorName"),
		_T("SQLNumResultCols"),
		_T("SQLPrepare"),
		_T("SQLRowCount"),
		_T("SQLSetCursorName"),
		_T("SQLSetParam"),
		_T("SQLTransact"),
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		_T("SQLColumns"),
		_T("SQLDriverConnect"),
		_T("SQLGetConnectOption"),
		_T("SQLGetData"),
		_T("SQLGetFunctions"),
		_T("SQLGetInfo"),
		_T("SQLGetStmtOption"),
		_T("SQLGetTypeInfo"),
		_T("SQLParamData"),
		_T("SQLPutData"),
		_T("SQLSetConnectOption"),
		_T("SQLSetStmtOption"),
		_T("SQLSpecialColumns"),
		_T("SQLStatistics"),
		_T("SQLTables"),
		_T("SQLBrowseConnect"),
		_T("SQLColumnPrivileges"),
		_T("SQLDataSources"),
		_T("SQLDescribeParam"),
		_T("SQLExtendedFetch"),
		_T("SQLForeignKeys"),
		_T("SQLMoreResults"),
		_T("SQLNativeSql"),
		_T("SQLNumParams"),
		_T("SQLParamOptions"),
		_T("SQLPrimaryKeys"),
		_T("SQLProcedureColumns"),
		_T("SQLProcedures"),
		_T("SQLSetPos"),
		_T("SQLSetScrollOptions"),
		_T("SQLTablePrivileges"),
		_T("SQLDrivers"),
		_T("SQLBindParameter"),
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
	};
	struct SqlFunction
	{
		LPCTSTR m_szName;
		SDWORD m_dwMask;
	};
	static const SqlFunction rgSqlFunctions[] =
	{
		// Conversion
		_T("Conversion"),		0xFFFF0000 | SQL_CONVERT_FUNCTIONS,
		_T("Convert()"),		SQL_FN_CVT_CONVERT,
		// Numeric
		_T("Numeric"),			0xFFFF0000 | SQL_NUMERIC_FUNCTIONS,
		_T("Abs()"),			SQL_FN_NUM_ABS,
		_T("Acos()"),			SQL_FN_NUM_ACOS,
		_T("Asin()"),			SQL_FN_NUM_ASIN,
		_T("Atan()"),			SQL_FN_NUM_ATAN,
		_T("Atan2()"),			SQL_FN_NUM_ATAN2,
		_T("Ceiling()"),		SQL_FN_NUM_CEILING,
		_T("Cos()"),			SQL_FN_NUM_COS,
		_T("Cot()"),			SQL_FN_NUM_COT,
		_T("Degrees()"),		SQL_FN_NUM_DEGREES,
		_T("Exp()"),			SQL_FN_NUM_EXP,
		_T("Floor()"),			SQL_FN_NUM_FLOOR,
		_T("Log()"),			SQL_FN_NUM_LOG,
		_T("Log10()"),			SQL_FN_NUM_LOG10,
		_T("Mod()"),			SQL_FN_NUM_MOD,
		_T("Pi()"),				SQL_FN_NUM_PI,
		_T("Power()"),			SQL_FN_NUM_POWER,
		_T("Radians()"),		SQL_FN_NUM_RADIANS,
		_T("Rand()"),			SQL_FN_NUM_RAND,
		_T("Round()"),			SQL_FN_NUM_ROUND,
		_T("Sign()"),			SQL_FN_NUM_SIGN,
		_T("Sin()"),			SQL_FN_NUM_SIN,
		_T("Sqrt()"),			SQL_FN_NUM_SQRT,
		_T("Tan()"),			SQL_FN_NUM_TAN,
		_T("Truncate()"),		SQL_FN_NUM_TRUNCATE,
		// String
		_T("String"),			0xFFFF0000 | SQL_STRING_FUNCTIONS,
		_T("Ascii()"),			SQL_FN_STR_ASCII,
		_T("Char()"),			SQL_FN_STR_CHAR,
		_T("Concat()"),			SQL_FN_STR_CONCAT,
		_T("Difference()"),		SQL_FN_STR_DIFFERENCE,
		_T("Insert()"),			SQL_FN_STR_INSERT,
		_T("LCase()"),			SQL_FN_STR_LCASE,
		_T("Left()"),			SQL_FN_STR_LEFT,
		_T("Lenght()"),			SQL_FN_STR_LENGTH,
		_T("Locate()"),			SQL_FN_STR_LOCATE,
		_T("Locate2()"),		SQL_FN_STR_LOCATE_2,
		_T("LTrim()"),			SQL_FN_STR_LTRIM,
		_T("Repeat()"),			SQL_FN_STR_REPEAT,
		_T("Replace()"),		SQL_FN_STR_REPLACE,
		_T("Right()"),			SQL_FN_STR_RIGHT,
		_T("RTrim()"),			SQL_FN_STR_RTRIM,
		_T("Soundex()"),		SQL_FN_STR_SOUNDEX,
		_T("Space()"),			SQL_FN_STR_SPACE,
		_T("Substring()"),		SQL_FN_STR_SUBSTRING,
		_T("UCase()"),			SQL_FN_STR_UCASE,
		// System
		_T("System"),			0xFFFF0000 | SQL_SYSTEM_FUNCTIONS,
		_T("DBName()"),			SQL_FN_SYS_DBNAME,
		_T("IfNull()"),			SQL_FN_SYS_IFNULL,
		_T("UserName()"),		SQL_FN_SYS_USERNAME,
		// Time/Date
		_T("Time/Date"),		0xFFFF0000 | SQL_TIMEDATE_FUNCTIONS,
		_T("CurDate()"),		SQL_FN_TD_CURDATE,
		_T("CurTime()"),		SQL_FN_TD_CURTIME,
		_T("DayName()"),		SQL_FN_TD_DAYNAME,
		_T("DayOfMonth()"),		SQL_FN_TD_DAYOFMONTH,
		_T("DayOfWeek()"),		SQL_FN_TD_DAYOFWEEK,
		_T("DayOfYear()"),		SQL_FN_TD_DAYOFYEAR,
		_T("Hour()"),			SQL_FN_TD_HOUR,
		_T("Minute()"),			SQL_FN_TD_MINUTE,
		_T("Month()"),			SQL_FN_TD_MONTH,
		_T("MonthName()"),		SQL_FN_TD_MONTHNAME,
		_T("Now()"),			SQL_FN_TD_NOW,
		_T("Quarter()"),		SQL_FN_TD_QUARTER,
		_T("Second()"),			SQL_FN_TD_SECOND,
		_T("TimeStampAdd()"),	SQL_FN_TD_TIMESTAMPADD,
		_T("TimeStampDiff()"),	SQL_FN_TD_TIMESTAMPDIFF,	
		_T("Week()"),			SQL_FN_TD_WEEK,
		_T("Year()"),			SQL_FN_TD_YEAR,
		_T("~0"),				0xFFFFFFFF
	};

	int i;
	UWORD aFunctions[RTL_NUMBER_OF(rgFunctions)];
	WriteTitle(_T("ODBC API Functions"));
	BeginTable();
	::SQLGetFunctions(GetHDBC(), SQL_API_ALL_FUNCTIONS, aFunctions);
	for (i = 1 ; i < RTL_NUMBER_OF(rgFunctions) ; ++i)
	{
		if (LPCTSTR szName = rgFunctions[i])
		{
			WriteValue(szName, aFunctions[i] ? szSupported : szNotSupported);
		}
	}
	SDWORD value;
	const SqlFunction *pSqlFunction = rgSqlFunctions;
	do
	{
		if (HIWORD(pSqlFunction->m_dwMask) == 0xFFFF)
		{
			EndTable();
			if (LOWORD(pSqlFunction->m_dwMask) != 0xFFFF)
			{
				TCHAR szText[80];
				wsprintf(szText, _T("SQL %s Functions"), pSqlFunction->m_szName);
				WriteTitle(szText);
				BeginTable();
				value = SQLGetInfoLong(LOWORD(pSqlFunction->m_dwMask));
			}
		}
		else
		{
			WriteValue(pSqlFunction->m_szName,
				value & pSqlFunction->m_dwMask ? szSupported : szNotSupported);
		}
	} while (pSqlFunction++->m_dwMask != 0xFFFFFFFF);
}

void CDBInfo::WriteSupportedSQL()
{
	CString string;
	DWORD dwValue;
	WriteTitle(_T("Supported SQL"));
	BeginTable();

	dwValue = SQLGetInfoLong(SQL_ALTER_TABLE);
	string.Format(_T("<none>%s%s"),
		&_T("\0,ADD")						[1 & dwValue / SQL_AT_ADD_COLUMN],
		&_T("\0,DROP")						[1 & dwValue / SQL_AT_DROP_COLUMN]);
	string.Replace(_T("<none>,"), _T(""));
	WriteValue(_T("ALTER TABLE Support"),	string);

	WriteValue(_T("Column Aliases"),		SQLGetInfoString(SQL_COLUMN_ALIAS),
											_T("NY\\Not Supported\\Supported"));

	C_ASSERT(SQL_CN_NONE == 0 && SQL_CN_DIFFERENT == 1 && SQL_CN_ANY == 2);
	WriteValue(_T("Correlation Names"),		SQLGetInfoShort(SQL_CORRELATION_NAME),
											_T("Not Supported\\")
											_T("Supported, table names must be different\\")
											_T("Supported"));
	
	WriteValue(_T("ORDER BY Expressions"),	SQLGetInfoString(SQL_EXPRESSIONS_IN_ORDERBY),
											_T("NY\\Not Supported\\Supported"));
	
	WriteValue(_T("GROUP BY NonAggregate"),	SQLGetInfoShort(SQL_GROUP_BY),
											_T("GROUP BY Not Supported\\")
											_T("GROUP BY Equals SELECT\\")
											_T("GROUP BY Contains SELECT\\")
											_T("No Relation\\")
											_T("Collate"));

	WriteValue(_T("LIKE Escape Clause"),	SQLGetInfoString(SQL_LIKE_ESCAPE_CLAUSE),
											_T("NY\\Not Supported\\Supported"));
	
	WriteValue(_T("ORDER BY in SELECT"),	SQLGetInfoString(SQL_ORDER_BY_COLUMNS_IN_SELECT),
											_T("NY\\No\\Yes"));

	WriteValue(_T("Outer Join Support"),	SQLGetInfoString(SQL_OUTER_JOINS),
											_T("NYPF\\No\\Yes\\Partial\\Full"));

	dwValue = SQLGetInfoLong(SQL_POSITIONED_STATEMENTS);
	string.Format(_T("<none>%s%s%s"),
		&_T("\0,Delete")					[1 & dwValue / SQL_PS_POSITIONED_DELETE],
		&_T("\0,Update")					[1 & dwValue / SQL_PS_POSITIONED_UPDATE],
		&_T("\0,Select for Update")			[1 & dwValue / SQL_PS_SELECT_FOR_UPDATE]);
	string.Replace(_T("<none>,"), _T(""));
	WriteValue(_T("Positioned Statements"), string);

	dwValue = SQLGetInfoLong(SQL_SUBQUERIES);
	string.Format(_T("<none>%s%s%s%s%s"),
		&_T("\0,Correlated")				[1 & dwValue / SQL_SQ_CORRELATED_SUBQUERIES],
		&_T("\0,Comparison")				[1 & dwValue / SQL_SQ_COMPARISON],
		&_T("\0,EXISTS")					[1 & dwValue / SQL_SQ_EXISTS],
		&_T("\0,IN")						[1 & dwValue / SQL_SQ_IN],
		&_T("\0,Quantified")				[1 & dwValue / SQL_SQ_QUANTIFIED]);
	string.Replace(_T("<none>,"), _T(""));
	WriteValue(_T("Subquery Support"), string);

	dwValue = SQLGetInfoLong(SQL_UNION);
	string.Format(_T("<none>%s%s"),
		&_T("\0,UNION")						[1 & dwValue / SQL_U_UNION],
		&_T("\0,UNION with ALL")			[1 & dwValue / SQL_U_UNION_ALL]);
	string.Replace(_T("<none>,"), _T(""));
	WriteValue(_T("UNION Support"), string);

	EndTable();
}

static LPCTSTR LegendDataType(int n)
{
	switch (n)
	{
	case SQL_BIGINT:			return _T("SQL_BIGINT");
	case SQL_BINARY:			return _T("SQL_BINARY");
	case SQL_BIT:				return _T("SQL_BIT");
	case SQL_CHAR:				return _T("SQL_CHAR");
	case SQL_DATE:				return _T("SQL_DATE");
	case SQL_DECIMAL:			return _T("SQL_DECIMAL");
	case SQL_DOUBLE:			return _T("SQL_DOUBLE");
	case SQL_FLOAT:				return _T("SQL_FLOAT");
	case SQL_GUID:				return _T("SQL_GUID");
	case SQL_INTEGER:			return _T("SQL_INTEGER");
	case SQL_LONGVARBINARY:		return _T("SQL_LONGVARBINARY");
	case SQL_LONGVARCHAR:		return _T("SQL_LONGVARCHAR");
	case SQL_NUMERIC:			return _T("SQL_NUMERIC");
	case SQL_REAL:				return _T("SQL_REAL");
	case SQL_SMALLINT:			return _T("SQL_SMALLINT");
	case SQL_TIME:				return _T("SQL_TIME");
	case SQL_TIMESTAMP:			return _T("SQL_TIMESTAMP");
	case SQL_TINYINT:			return _T("SQL_TINYINT");
	case SQL_VARBINARY:			return _T("SQL_VARBINARY");
	case SQL_VARCHAR:			return _T("SQL_VARCHAR");
	case SQL_WCHAR:				return _T("SQL_WCHAR");
	case SQL_WLONGVARCHAR:		return _T("SQL_WLONGVARCHAR");
	case SQL_WVARCHAR:			return _T("SQL_WVARCHAR");
	}
	return 0;
}

static UWORD ConvertInfoIndex(int n)
{
	switch (n)
	{
	case SQL_BIGINT:			return SQL_CONVERT_BIGINT;
	case SQL_BINARY:			return SQL_CONVERT_BINARY;
	case SQL_BIT:				return SQL_CONVERT_BIT;
	case SQL_CHAR:				return SQL_CONVERT_CHAR;
	case SQL_DATE:				return SQL_CONVERT_DATE;
	case SQL_DECIMAL:			return SQL_CONVERT_DECIMAL;
	case SQL_DOUBLE:			return SQL_CONVERT_DOUBLE;
	case SQL_FLOAT:				return SQL_CONVERT_FLOAT;
	case SQL_INTEGER:			return SQL_CONVERT_INTEGER;
	case SQL_LONGVARBINARY:		return SQL_CONVERT_LONGVARBINARY;
	case SQL_LONGVARCHAR:		return SQL_CONVERT_LONGVARCHAR;
	case SQL_NUMERIC:			return SQL_CONVERT_NUMERIC;
	case SQL_REAL:				return SQL_CONVERT_REAL;
	case SQL_SMALLINT:			return SQL_CONVERT_SMALLINT;
	case SQL_TIME:				return SQL_CONVERT_TIME;
	case SQL_TIMESTAMP:			return SQL_CONVERT_TIMESTAMP;
	case SQL_TINYINT:			return SQL_CONVERT_TINYINT;
	case SQL_VARBINARY:			return SQL_CONVERT_VARBINARY;
	case SQL_VARCHAR:			return SQL_CONVERT_VARCHAR;
	}
	return 0;
}

static LPCTSTR LegendSearchable(int n)
{
	switch (n)
	{
	case SQL_UNSEARCHABLE:		return _T("No");
	case SQL_LIKE_ONLY:			return _T("Only with LIKE");
	case SQL_ALL_EXCEPT_LIKE:	return _T("with all except LIKE");
	case SQL_SEARCHABLE:		return _T("Yes");
	}
	return 0;
}

static LPCTSTR LegendNullable(int n)
{
	switch (n)
	{
	case SQL_NO_NULLS:			return _T("No");
	case SQL_NULLABLE:			return _T("Yes");
	case SQL_NULLABLE_UNKNOWN:	return _T("Unknown");
	}
	return 0;
}

void CDBInfo::WriteDataTypes()
{
	enum
	{
		TYPE_NAME,
		DATA_TYPE,
		PRECISION,
		LITERAL_PREFIX,
		LITERAL_SUFFIX,
		CREATE_PARAMS,
		NULLABLE,
		CASE_SENSITIVE,
		SEARCHABLE,
		UNSIGNED_ATTRIBUTE,
		MONEY,
		AUTO_INCREMENT,
		LOCAL_TYPE_NAME,
		MINIMUM_SCALE,
		MAXIMUM_SCALE
	};

	CObList rgRows;
	CRecordsetEx *prs = CRecordsetEx::New(m_pDatabase);
	prs->m_lOpen = SQL_API_SQLGETTYPEINFO;
	prs->Execute();
	while (!prs->IsEOF())
	{
		rgRows.AddTail(CDBRow::Scan(*prs));
		prs->MoveNext();
	}
	prs->Release();

	POSITION pos;
	int iVal;

	WriteTitle(_T("Data Types"));
	CDBRow::WriteString(m_pfText, _T("\n"));

	BeginTable();
	WriteTableHeader(_T("Native type         SQL Type          Precision   Min Scale  Max Scale"));
	pos = rgRows.GetHeadPosition();
	while (pos)
	{
		CDBRow::Handle *pRow = static_cast<CDBRow::Handle *>(rgRows.GetNext(pos));
		CString rgsRow[] =
		{
			pRow->Item(TYPE_NAME)->asString(),
			pRow->Item(DATA_TYPE)->asString(),
			pRow->Item(PRECISION)->asString(_T("<n/a>")),
			pRow->Item(MINIMUM_SCALE)->asString(_T("<n/a>")),
			pRow->Item(MAXIMUM_SCALE)->asString(_T("<n/a>"))
		};
		if (LPCTSTR szLegend = LegendDataType(StrToInt(rgsRow[1])))
			rgsRow[1] = szLegend;
		WriteTableRow(_T("%-20s%-18s%-12s%-11s%s\n"), rgsRow);
	}
	EndTable();

	BeginTable();
	WriteTableHeader(_T("Native type         Nullable  Unsigned  Auto Inc  Money  Case Sensitive"));
	pos = rgRows.GetHeadPosition();
	while (pos)
	{
		CDBRow::Handle *pRow = static_cast<CDBRow::Handle *>(rgRows.GetNext(pos));
		CString rgsRow[] =
		{
			pRow->Item(TYPE_NAME)->asString(),
			pRow->Item(NULLABLE)->asString(),
			pRow->Item(UNSIGNED_ATTRIBUTE)->asString(_T("<n/a>")),
			pRow->Item(AUTO_INCREMENT)->asString(_T("<n/a>")),
			pRow->Item(MONEY)->asString(),
			pRow->Item(CASE_SENSITIVE)->asString()
		};
		if (LPCTSTR szLegend = LegendNullable(StrToInt(rgsRow[1])))
			rgsRow[1] = szLegend;
		if (StrToIntEx(rgsRow[2], STIF_DEFAULT, &iVal))
			rgsRow[2] = iVal ? _T("Yes") : _T("No");
		if (StrToIntEx(rgsRow[3], STIF_DEFAULT, &iVal))
			rgsRow[3] = iVal ? _T("Yes") : _T("No");
		if (StrToIntEx(rgsRow[4], STIF_DEFAULT, &iVal))
			rgsRow[4] = iVal ? _T("Yes") : _T("No");
		if (StrToIntEx(rgsRow[5], STIF_DEFAULT, &iVal))
			rgsRow[5] = iVal ? _T("Yes") : _T("No");
		WriteTableRow(_T("%-20s%-10s%-10s%-10s%-7s%s\n"), rgsRow);
	}
	EndTable();

	BeginTable();
	WriteTableHeader(_T("Native type         Lit. Prefix,Suffix  Create Params    Searchable"));
	pos = rgRows.GetHeadPosition();
	while (pos)
	{
		CDBRow::Handle *pRow = static_cast<CDBRow::Handle *>(rgRows.GetNext(pos));
		CString rgsRow[] =
		{
			pRow->Item(TYPE_NAME)->asString(),
			pRow->Item(LITERAL_PREFIX)->asString(_T("<n/a>")) +
			',' + pRow->Item(LITERAL_SUFFIX)->asString(_T("<n/a>")),
			pRow->Item(CREATE_PARAMS)->asString(_T("<none>")),
			pRow->Item(SEARCHABLE)->asString()
		};
		if (LPCTSTR szLegend = LegendSearchable(StrToInt(rgsRow[3])))
			rgsRow[3] = szLegend;
		WriteTableRow(_T("%-20s%-20s%-17s%s\n"), rgsRow);
	}
	EndTable();

	BeginTable();
	WriteTableHeader(_T("Native type         Convert() Conversions"));
	pos = rgRows.GetHeadPosition();
	while (pos)
	{
		CDBRow::Handle *pRow = static_cast<CDBRow::Handle *>(rgRows.GetNext(pos));
		CString name = pRow->Item(TYPE_NAME)->asString();
		EnsureAnsi(name);
		CString str;
		str.Format(_T("%-20s"), name);
		CDBRow::WriteString(m_pfText, str);
		CDBRow::WriteString(m_pfHtml, _T("<tr><td>"));
		CDBRow::WriteString(m_pfHtml, str);
		CDBRow::WriteString(m_pfHtml, _T("</td><td>"));
		iVal = StrToInt(pRow->Item(DATA_TYPE)->asString());
		DWORD value = 0;
		if (UWORD wIndex = ConvertInfoIndex(iVal))
			value = SQLGetInfoLong(wIndex);
		if (value)
		{
			int ich = 0;
			if (value & SQL_CVT_BIGINT)
				ich = AppendValue(ich, _T("SQL_BIGINT"));
			if (value & SQL_CVT_BINARY)
				ich = AppendValue(ich, _T("SQL_BINARY"));
			if (value & SQL_CVT_BIT)
				ich = AppendValue(ich, _T("SQL_BIT"));
			if (value & SQL_CVT_CHAR)
				ich = AppendValue(ich, _T("SQL_CHAR"));
			if (value & SQL_CVT_DATE)
				ich = AppendValue(ich, _T("SQL_DATE"));
			if (value & SQL_CVT_DECIMAL)
				ich = AppendValue(ich, _T("SQL_DECIMAL"));
			if (value & SQL_CVT_DOUBLE)
				ich = AppendValue(ich, _T("SQL_DOUBLE"));
			if (value & SQL_CVT_FLOAT)
				ich = AppendValue(ich, _T("SQL_FLOAT"));
			if (value & SQL_CVT_INTEGER)
				ich = AppendValue(ich, _T("SQL_INTEGER"));
			if (value & SQL_CVT_INTERVAL_DAY_TIME)
				ich = AppendValue(ich, _T("SQL_INTERVAL_DAY_TO_SECOND"));
			if (value & SQL_CVT_INTERVAL_YEAR_MONTH)
				ich = AppendValue(ich, _T("SQL_INTERVAL_YEAR_TO_MONTH"));
			if (value & SQL_CVT_LONGVARBINARY)
				ich = AppendValue(ich, _T("SQL_LONGVARBINARY"));
			if (value & SQL_CVT_LONGVARCHAR)
				ich = AppendValue(ich, _T("SQL_LONGVARCHAR"));
			if (value & SQL_CVT_NUMERIC)
				ich = AppendValue(ich, _T("SQL_NUMERIC"));
			if (value & SQL_CVT_REAL)
				ich = AppendValue(ich, _T("SQL_REAL"));
			if (value & SQL_CVT_SMALLINT)
				ich = AppendValue(ich, _T("SQL_SMALLINT"));
			if (value & SQL_CVT_TIME)
				ich = AppendValue(ich, _T("SQL_TIME"));
			if (value & SQL_CVT_TIMESTAMP)
				ich = AppendValue(ich, _T("SQL_TIMESTAMP"));
			if (value & SQL_CVT_TINYINT)
				ich = AppendValue(ich, _T("SQL_TINYINT"));
			if (value & SQL_CVT_VARBINARY)
				ich = AppendValue(ich, _T("SQL_VARBINARY"));
			if (value & SQL_CVT_VARCHAR)
				ich = AppendValue(ich, _T("SQL_VARCHAR"));
			if (value & SQL_CVT_WCHAR)
				ich = AppendValue(ich, _T("SQL_WCHAR"));
			if (value & SQL_CVT_WLONGVARCHAR)
				ich = AppendValue(ich, _T("SQL_WLONGVARCHAR"));
			if (value & SQL_CVT_WVARCHAR)
				ich = AppendValue(ich, _T("SQL_WVARCHAR"));
		}
		else
		{
			str = _T("<none>");
			CDBRow::WriteString(m_pfText, str);
			CDBRow::MakeHtmlEntities(str);
			CDBRow::WriteString(m_pfHtml, str);
		}
		CDBRow::WriteString(m_pfHtml, _T("</td></tr>\n"));
		CDBRow::WriteString(m_pfText, _T("\n\n"));
	}
	EndTable();

	while (!rgRows.IsEmpty())
		delete rgRows.RemoveHead();
}

void CDBInfo::WriteIdentifiers()
{
	WriteTitle(_T("SQL Keywords"));
	WriteKeywords(SQL_ODBC_KEYWORDS);

	char buffer[32000];
	SWORD cbData;
	if SQL_SUCCEEDED(::SQLGetInfo(GetHDBC(), SQL_KEYWORDS, buffer, sizeof buffer, &cbData))
	{
		WriteTitle(_T("Driver-Specific Keywords"));
		WriteKeywords(buffer);
	}

	WriteTitle(_T("Identifiers"));
	BeginTable();
	WriteValue(_T("Identifier Case"),		SQLGetInfoShort(SQL_IDENTIFIER_CASE),
											_T("<n/a>\\Upper\\Lower\\Sensitive\\Mixed"));
	WriteValue(_T("Quoted Identifier Case"),SQLGetInfoShort(SQL_QUOTED_IDENTIFIER_CASE),
											_T("<n/a>\\Upper\\Lower\\Sensitive\\Mixed"));
	WriteValue(_T("Search Pattern Escape"),	SQLGetInfoString(SQL_SEARCH_PATTERN_ESCAPE));
	WriteValue(_T("Identifier Quote Char"),	SQLGetInfoString(SQL_IDENTIFIER_QUOTE_CHAR));
	WriteValue(_T("Catalog Location"),		SQLGetInfoShort(SQL_CATALOG_LOCATION),
											_T("<n/a>\\Start\\End"));
	WriteValue(_T("Catalog Separator"),		SQLGetInfoString(SQL_CATALOG_NAME_SEPARATOR));
	WriteValue(_T("Catalog Term"),			SQLGetInfoString(SQL_CATALOG_TERM));
	WriteValue(_T("Schema Term"),			SQLGetInfoString(SQL_SCHEMA_TERM));
	WriteValue(_T("Procedure Term"),		SQLGetInfoString(SQL_PROCEDURE_TERM));
	WriteValue(_T("TableTerm"),				SQLGetInfoString(SQL_TABLE_TERM));
	EndTable();

	WriteTitle(_T("Special Characters"));
	CDBRow::WriteString(m_pfHtml, _T("<p>\n"));
	CString strSqlSpecialCharacters = SQLGetInfoString(SQL_SPECIAL_CHARACTERS);
	if (int cch = strSqlSpecialCharacters.GetLength())
	{
		LPCTSTR pch = strSqlSpecialCharacters;
		while (cch > 75)
		{
			CString str(pch, 75);
			CDBRow::WriteString(m_pfText, str);
			CDBRow::WriteString(m_pfText, _T("\n"));
			//CDBRow::WriteString(pfText, CString(' ', 28));
			CDBRow::MakeHtmlEntities(str);
			CDBRow::WriteString(m_pfHtml, str);
			CDBRow::WriteString(m_pfHtml, _T("<br>\n"));
			pch += 75;
			cch -= 75;
		}
		CString str(pch, 75);
		CDBRow::WriteString(m_pfText, str);
		CDBRow::MakeHtmlEntities(str);
		CDBRow::WriteString(m_pfHtml, str);
	}
	else
	{
		CString str = _T("<none>");
		CDBRow::WriteString(m_pfText, str);
		CDBRow::MakeHtmlEntities(str);
		CDBRow::WriteString(m_pfHtml, str);
	}
	CDBRow::WriteString(m_pfText, _T("\n\n"));
	CDBRow::WriteString(m_pfHtml, _T("</p>\n"));
}

void CDBInfo::WriteLimits()
{
	static const LPCTSTR rgIntervalNames[] =
	{
		_T("Second Fraction"),
		_T("Second"),
		_T("Minute"),
		_T("Hour"),
		_T("Day"),
		_T("Week"),
		_T("Month"),
		_T("Quarter"),
		_T("Year")
	};

	WriteTitle(_T("Limits"));
	BeginTable();
	WriteValue(_T("Max Binary Literal Length"),	SQLGetInfoLong(SQL_MAX_BINARY_LITERAL_LEN),
												_T("<none>"));
	WriteValue(_T("Max Catalog Name Length"),	SQLGetInfoShort(SQL_MAX_QUALIFIER_NAME_LEN),
												_T("<none>"));
	WriteValue(_T("Max Char Literal Length"),	SQLGetInfoLong(SQL_MAX_CHAR_LITERAL_LEN),
												_T("<none>"));
	WriteValue(_T("Max Column Name Length"),	SQLGetInfoShort(SQL_MAX_COLUMN_NAME_LEN),
												_T("<none>"));
	WriteValue(_T("Max Columns in GROUP BY"),	SQLGetInfoShort(SQL_MAX_COLUMNS_IN_GROUP_BY),
												_T("<none>"));
	WriteValue(_T("Max Columns in Index"),		SQLGetInfoShort(SQL_MAX_COLUMNS_IN_INDEX),
												_T("<none>"));
	WriteValue(_T("Max Columns in ORDER BY"),	SQLGetInfoShort(SQL_MAX_COLUMNS_IN_ORDER_BY),
												_T("<none>"));
	WriteValue(_T("Max Columns in SELECT"),		SQLGetInfoShort(SQL_MAX_COLUMNS_IN_SELECT),
												_T("<none>"));
	WriteValue(_T("Max Columns in Table"),		SQLGetInfoShort(SQL_MAX_COLUMNS_IN_TABLE),
												_T("<none>"));
	WriteValue(_T("Max Cursor Name Length"),	SQLGetInfoShort(SQL_MAX_CURSOR_NAME_LEN),
												_T("<none>"));
	WriteValue(_T("Max Index Size"),			SQLGetInfoLong(SQL_MAX_INDEX_SIZE),
												_T("<none>"));
	WriteValue(_T("Max Procedure Name Length"), SQLGetInfoShort(SQL_MAX_PROCEDURE_NAME_LEN),
												_T("<none>"));
	WriteValue(_T("Max Row Size"),				SQLGetInfoLong(SQL_MAX_ROW_SIZE),
												_T("<none>"));
	WriteValue(_T("Max Rowsize Includes Long"),	SQLGetInfoString(SQL_MAX_ROW_SIZE_INCLUDES_LONG),
												_T("NY\\No\\Yes"));
	WriteValue(_T("Max Schema Name Length"),	SQLGetInfoShort(SQL_MAX_OWNER_NAME_LEN),
												_T("<none>"));
	WriteValue(_T("Max Statement Length"),		SQLGetInfoLong(SQL_MAX_STATEMENT_LEN),
												_T("<none>"));
	WriteValue(_T("Max Table Name Length"),		SQLGetInfoShort(SQL_MAX_TABLE_NAME_LEN),
												_T("<none>"));
	WriteValue(_T("Max Tables In SELECT"),		SQLGetInfoShort(SQL_MAX_TABLES_IN_SELECT),
												_T("<none>"));
	WriteValue(_T("Max User Name Length"),		SQLGetInfoShort(SQL_MAX_USER_NAME_LEN),
												_T("<none>"));
	// Timestamp Add Intervals
	CDBRow::WriteString(m_pfText, _T("  Timestamp Add Intervals - "));
	CDBRow::WriteString(m_pfHtml, _T("<tr><td>Timestamp Add Intervals</td><td>"));
	if (DWORD value =
		SQLGetInfoLong(SQL_TIMEDATE_FUNCTIONS) & SQL_FN_TD_TIMESTAMPADD ?
		SQLGetInfoLong(SQL_TIMEDATE_ADD_INTERVALS) : 0)
	{
		int ich = 0;
		for (int i = 0 ; i < RTL_NUMBER_OF(rgIntervalNames) ; ++i)
		{
			if (value & (1 << i))
			{
				ich = AppendValue(ich, rgIntervalNames[i], 28);
			}
		}
	}
	else
	{
		CString str = _T("<n/a>");
		CDBRow::WriteString(m_pfText, str);
		CDBRow::MakeHtmlEntities(str);
		CDBRow::WriteString(m_pfHtml, str);
	}
	CDBRow::WriteString(m_pfHtml, _T("</td></tr>\n"));
	CDBRow::WriteString(m_pfText, _T("\n"));
	// Timestamp Diff Intervals
	CDBRow::WriteString(m_pfText, _T(" Timestamp Diff Intervals - "));
	CDBRow::WriteString(m_pfHtml, _T("<tr><td>Timestamp Add Intervals</td><td>"));
	if (DWORD value =
		SQLGetInfoLong(SQL_TIMEDATE_FUNCTIONS) & SQL_FN_TD_TIMESTAMPDIFF ?
		SQLGetInfoLong(SQL_TIMEDATE_DIFF_INTERVALS) : 0)
	{
		int ich = 0;
		for (int i = 0 ; i < RTL_NUMBER_OF(rgIntervalNames) ; ++i)
		{
			if (value & (1 << i))
			{
				ich = AppendValue(ich, rgIntervalNames[i], 28);
			}
		}
	}
	else
	{
		CString str = _T("<n/a>");
		CDBRow::WriteString(m_pfText, str);
		CDBRow::MakeHtmlEntities(str);
		CDBRow::WriteString(m_pfHtml, str);
	}
	CDBRow::WriteString(m_pfText, _T("\n"));
	CDBRow::WriteString(m_pfHtml, _T("</td></tr>\n"));
	EndTable();
}

void CDBInfo::WriteTransactions()
{
	CString string;
	DWORD dwValue;
	WriteTitle(_T("Transactions"));
	BeginTable();
	WriteValue(_T("Transaction Support"),		SQLGetInfoShort(SQL_TXN_CAPABLE),
												_T("None\\")
												_T("DDL statements cause an error\\")
												_T("DDL and DML statements supported\\")
												_T("DDL statements cause a commit\\")
												_T("DDL statements are ignored"));
	WriteValue(_T("Cursor Commit Behavior"),	SQLGetInfoShort(SQL_CURSOR_COMMIT_BEHAVIOR),
												_T("Close Cursors, Delete Statements\\")
												_T("Close Cursors\\")
												_T("Preserve Cursors"));
	WriteValue(_T("Cursor Rollback Behavior"),	SQLGetInfoShort(SQL_CURSOR_ROLLBACK_BEHAVIOR),
												_T("Close Cursors, Delete Statements\\")
												_T("Close Cursors\\")
												_T("Preserve Cursors"));

	dwValue = SQLGetInfoLong(SQL_TXN_ISOLATION_OPTION);
	string.Format(_T("<none>%s%s%s%s"),
		&_T("\0,Read Commited")					[1 & dwValue / SQL_TXN_READ_COMMITTED],
		&_T("\0,Read Uncommited")				[1 & dwValue / SQL_TXN_READ_UNCOMMITTED],
		&_T("\0,Repeatable Read")				[1 & dwValue / SQL_TXN_REPEATABLE_READ],
		&_T("\0,Serializable")					[1 & dwValue / SQL_TXN_SERIALIZABLE]);
	string.Replace(_T("<none>,"), _T(""));
	WriteValue(_T("Trans. Isolation Levels"), string);

	dwValue = SQLGetInfoLong(SQL_DEFAULT_TXN_ISOLATION);
	string.Format(_T("<none>%s%s%s%s"),
		&_T("\0,Read Commited")					[1 & dwValue / SQL_TXN_READ_COMMITTED],
		&_T("\0,Read Uncommited")				[1 & dwValue / SQL_TXN_READ_UNCOMMITTED],
		&_T("\0,Repeatable Read")				[1 & dwValue / SQL_TXN_REPEATABLE_READ],
		&_T("\0,Serializable")					[1 & dwValue / SQL_TXN_SERIALIZABLE]);
	string.Replace(_T("<none>,"), _T(""));
	WriteValue(_T("Default Trans. Isolation"), string);

	WriteValue(_T("Multiple Active Trans."),	SQLGetInfoString(SQL_MULTIPLE_ACTIVE_TXN),
												_T("NY\\Not Supported\\Supported"));

	dwValue = SQLGetInfoLong(SQL_BOOKMARK_PERSISTENCE);
	string.Format(_T("<none>%s%s%s%s%s%s%s"),
		&_T("\0,Close")							[1 & dwValue / SQL_BP_CLOSE],
		&_T("\0,Delete")						[1 & dwValue / SQL_BP_DELETE],
		&_T("\0,Drop")							[1 & dwValue / SQL_BP_DROP],
		&_T("\0,Other HSTMT")					[1 & dwValue / SQL_BP_OTHER_HSTMT],
		&_T("\0,Scroll")						[1 & dwValue / SQL_BP_SCROLL],
		&_T("\0,Transaction")					[1 & dwValue / SQL_BP_TRANSACTION],
		&_T("\0,Update")						[1 & dwValue / SQL_BP_UPDATE]);
	string.Replace(_T("<none>,"), _T(""));
	WriteValue(_T("Bookmark Persistance"), string);

	dwValue = SQLGetInfoLong(SQL_LOCK_TYPES);
	string.Format(_T("<none>%s%s%s"),
		&_T("\0,Exclusive")						[1 & dwValue / SQL_LCK_EXCLUSIVE],
		&_T("\0,No Change")						[1 & dwValue / SQL_LCK_NO_CHANGE],
		&_T("\0,Unlock")						[1 & dwValue / SQL_LCK_UNLOCK]);
	string.Replace(_T("<none>,"), _T(""));
	WriteValue(_T("Lock Types"), string);

	dwValue = SQLGetInfoLong(SQL_STATIC_SENSITIVITY);
	string.Format(_T("<none>%s%s%s"),
		&_T("\0,Additions")						[1 & dwValue / SQL_SS_ADDITIONS],
		&_T("\0,Deletions")						[1 & dwValue / SQL_SS_DELETIONS],
		&_T("\0,Updates")						[1 & dwValue / SQL_SS_UPDATES]);
	string.Replace(_T("<none>,"), _T(""));
	WriteValue(_T("Static Sensitivity"), string);
	EndTable();
}

void CDBInfo::WriteMiscellaneous()
{
	CString string;
	DWORD dwValue;
	WriteTitle(_T("Miscellaneous"));
	BeginTable();

	WriteValue(_T("Need Long Data Length"),		SQLGetInfoString(SQL_NEED_LONG_DATA_LEN),
												_T("NY\\No\\Yes"));
	WriteValue(_T("Non-Nullable Columns"),		SQLGetInfoShort(SQL_NON_NULLABLE_COLUMNS),
												_T("Not Supported\\Supported"));
	WriteValue(_T("Null Concatenation"),		SQLGetInfoShort(SQL_CONCAT_NULL_BEHAVIOR),
												_T("Result is NULL\\Result is not NULL"));
	WriteValue(_T("Null Collation"),			SQLGetInfoShort(SQL_NULL_COLLATION),
												_T("High\\Low\\Start\\3\\End"));
	WriteValue(_T("Procedures"),				SQLGetInfoString(SQL_PROCEDURES),
												_T("NY\\Not Supported\\Supported"));
	WriteValue(_T("Row Updates"),				SQLGetInfoString(SQL_ROW_UPDATES),
												_T("NY\\No\\Yes"));

	dwValue = SQLGetInfoLong(SQL_FETCH_DIRECTION);
	string.Format(_T("<none>%s%s%s%s%s%s%s"),
		&_T("\0,Absolute")						[1 & dwValue / SQL_FD_FETCH_ABSOLUTE],
		&_T("\0,Bookmark")						[1 & dwValue / SQL_FD_FETCH_BOOKMARK],
		&_T("\0,First")							[1 & dwValue / SQL_FD_FETCH_FIRST],
		&_T("\0,Last")							[1 & dwValue / SQL_FD_FETCH_LAST],
		&_T("\0,Next")							[1 & dwValue / SQL_FD_FETCH_NEXT],
		&_T("\0,Prior")							[1 & dwValue / SQL_FD_FETCH_PRIOR],
		&_T("\0,Relative")						[1 & dwValue / SQL_FD_FETCH_RELATIVE]);
	string.Replace(_T("<none>,"), _T(""));
	WriteValue(_T("Fetch Direction"), string);

	dwValue = SQLGetInfoLong(SQL_GETDATA_EXTENSIONS);
	string.Format(_T("<none>%s%s%s%s"),
		&_T("\0,Any Column")					[1 & dwValue / SQL_GD_ANY_COLUMN],
		&_T("\0,Any Order")						[1 & dwValue / SQL_GD_ANY_ORDER],
		&_T("\0,Block")							[1 & dwValue / SQL_GD_BLOCK],
		&_T("\0,Bound")							[1 & dwValue / SQL_GD_BOUND]);
	string.Replace(_T("<none>,"), _T(""));
	WriteValue(_T("Get Data Extensions"), string);

	dwValue = SQLGetInfoLong(SQL_POS_OPERATIONS);
	string.Format(_T("<none>%s%s%s%s%s"),
		&_T("\0,Add")							[1 & dwValue / SQL_POS_ADD],
		&_T("\0,Delete")						[1 & dwValue / SQL_POS_DELETE],
		&_T("\0,Position")						[1 & dwValue / SQL_POS_POSITION],
		&_T("\0,Refresh")						[1 & dwValue / SQL_POS_REFRESH],
		&_T("\0,Update")						[1 & dwValue / SQL_POS_UPDATE]);
	string.Replace(_T("<none>,"), _T(""));
	WriteValue(_T("Positioned Operations"), string);

	dwValue = SQLGetInfoLong(SQL_SCROLL_CONCURRENCY);
	string.Format(_T("<none>%s%s%s%s"),
		&_T("\0,Lock")							[1 & dwValue / SQL_SCCO_LOCK],
		&_T("\0,Opt/RowVer")					[1 & dwValue / SQL_SCCO_OPT_ROWVER],
		&_T("\0,Opt/Values")					[1 & dwValue / SQL_SCCO_OPT_VALUES],
		&_T("\0,Read Only")						[1 & dwValue / SQL_SCCO_READ_ONLY]);
	string.Replace(_T("<none>,"), _T(""));
	WriteValue(_T("Scroll Concurrency"), string);

	dwValue = SQLGetInfoLong(SQL_SCROLL_OPTIONS);
	string.Format(_T("<none>%s%s%s%s%s"),
		&_T("\0,Dynamic")						[1 & dwValue / SQL_SO_DYNAMIC],
		&_T("\0,Forward Only")					[1 & dwValue / SQL_SO_FORWARD_ONLY],
		&_T("\0,Keyset Driven")					[1 & dwValue / SQL_SO_KEYSET_DRIVEN],
		&_T("\0,Mixed")							[1 & dwValue / SQL_SO_MIXED],
		&_T("\0,Static")						[1 & dwValue / SQL_SO_STATIC]);
	string.Replace(_T("<none>,"), _T(""));
	WriteValue(_T("Scroll Options"), string);

	dwValue = SQLGetInfoLong(SQL_SCHEMA_USAGE);
	string.Format(_T("<none>%s%s%s%s%s"),
		&_T("\0,DML Stmt")						[1 & dwValue / SQL_SU_DML_STATEMENTS],
		&_T("\0,Index Def")						[1 & dwValue / SQL_SU_INDEX_DEFINITION],
		&_T("\0,Priv Def")						[1 & dwValue / SQL_SU_PRIVILEGE_DEFINITION],
		&_T("\0,Proc Invoke")					[1 & dwValue / SQL_SU_PROCEDURE_INVOCATION],
		&_T("\0,Table Def")						[1 & dwValue / SQL_SU_TABLE_DEFINITION]);
	string.Replace(_T("<none>,"), _T(""));
	WriteValue(_T("Schema Usage"), string);

	dwValue = SQLGetInfoLong(SQL_CATALOG_USAGE);
	string.Format(_T("<none>%s%s%s%s%s"),
		&_T("\0,DML Stmt")						[1 & dwValue / SQL_CU_DML_STATEMENTS],
		&_T("\0,Index Def")						[1 & dwValue / SQL_CU_INDEX_DEFINITION],
		&_T("\0,Priv Def")						[1 & dwValue / SQL_CU_PRIVILEGE_DEFINITION],
		&_T("\0,Proc Invoke")					[1 & dwValue / SQL_CU_PROCEDURE_INVOCATION],
		&_T("\0,Table Def")						[1 & dwValue / SQL_CU_TABLE_DEFINITION]);
	string.Replace(_T("<none>,"), _T(""));
	WriteValue(_T("Catalog Usage"), string);

	EndTable();
}

