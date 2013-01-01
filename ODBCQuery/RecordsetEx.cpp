/*/RecordsetEx.cpp

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
#include "stdafx.h"
#include "DatabaseEx.h"
#include "RecordsetEx.h"
#include "Helpers.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRecordsetEx

void CRecordsetEx::ReplaceBrackets(CString &strSQL)
{
	m_pDatabase->ReplaceBrackets(strSQL.LockBuffer());
	strSQL.UnlockBuffer();
}

int CRecordsetEx::LookAheadSQL(LPCTSTR pch, TCHAR term)
{
	int cch = 0;
	int quote = 0;
	int depth = 0;
	int opening = -1;
	int closing = -1;
recurse:
	switch (int c = pch[cch])
	{
	case '\'': case '"':
		if (quote == 0 || quote == c)
			quote ^= c;
		break;
	case '(': case '[': case '{':
		if (quote == 0 && depth++ == 0)
			opening = cch;
		break;
	case ')': case ']': case '}':
		if (quote == 0 && --depth == 0)
			closing = cch;
		break;
	default:
		if (c == term && quote == 0 && depth == 0)
		{
		case '\0':
			if (opening == 0 && closing == cch - 1)
				cch = -cch;
			return cch;
		}
		break;
	}
	++cch;
	goto recurse;
}

LPTSTR CRecordsetEx::LookAheadTerminateSQL(LPTSTR pch)
{
	int cch = LookAheadSQL(pch);
	if (cch < 0)
	{
		cch = -cch;
		*pch = ';';
		pch[cch - 1] = '\0';
	}
	if (*(pch += cch) == ';')
		*pch++ = '\0';
	return pch;
}

int CRecordsetEx::IsPrefix(const CString &str, LPCTSTR pch)
{
	int cch = lstrlen(pch);
	if (str.GetLength() < cch ||
		CompareString(0, NORM_IGNORECASE, str, cch, pch, cch) != CSTR_EQUAL)
	{
		cch = 0;
	}
	return cch;
}

CRecordsetEx *CRecordsetEx::New(CDatabaseEx *pDatabase)
{
	CRecordsetEx *prs = static_cast<CRecordsetEx *>(new CRecordset);
	prs->m_pDatabase = pDatabase;
	pDatabase->m_listRecordsets.AddTail(prs);
	return prs;
}

inline SQLCHAR *SQLNTS(LPCTSTR psz)
{
	if (psz)
	{
		if (*psz == ';')
			++psz;
		if (*psz == '\0')
			psz = 0;
	}
	return (SQLCHAR *)psz;
}

inline UWORD SQLNUM(LPCTSTR psz, UWORD defval)
{
	if (psz)
	{
		if (*psz == ';')
			++psz;
		if (*psz == '\0')
			psz = 0;
	}
	return psz ? (UWORD)StrToInt(psz) : defval;
}

void CRecordsetEx::BuildSelect(SQLCHAR *major, SQLCHAR *minor)
{
	if (major)
		m_strSQL.Format(_T("SELECT * FROM [%s].[%s]"), major, minor);
	else
		m_strSQL.Format(_T("SELECT * FROM [%s]"), minor);
	if (static_cast<CDatabaseEx *>(m_pDatabase)->m_listRecordsets.Find(this) == NULL)
		static_cast<CDatabaseEx *>(m_pDatabase)->m_listRecordsets.AddHead(this);
}

CRecordsetEx::ExecuteStatus CRecordsetEx::Execute(int nOpenType)
{
	switch (m_lOpen)
	{
	case AFX_RECORDSET_STATUS_OPEN:
		return pending;
	case 9999:
		ReplaceBrackets(m_strSQL);
		return skipped;
	}

	ReplaceBrackets(m_strTableName);

	ExecuteStatus status;

	CMapStringToPtr rgPK;
	CString strSQL;
	do
	{
		status = initial;
		if (m_hstmt == SQL_NULL_HSTMT &&
			!Check(::SQLAllocStmt(m_pDatabase->m_hdbc, &m_hstmt)))
		{
			ThrowDBException(SQL_INVALID_HANDLE);
		}
		SetState(nOpenType, 0, executeDirect);
		OnSetOptions(m_hstmt);

		switch (long lOpen = m_lOpen)
		{
		case SQL_API_SQLTABLES:
			{
				status = initial_catalog;

				LPTSTR pch;
				strSQL = m_strSQL;
				LPCTSTR pszTableQualifier	= pch = strSQL.LockBuffer();
				LPCTSTR pszTableOwner		= pch = LookAheadTerminateSQL(pch);
				LPCTSTR pszTableName		= pch = LookAheadTerminateSQL(pch);
				LPCTSTR pszTableType		= pch = LookAheadTerminateSQL(pch);
				LookAheadTerminateSQL(pch);

				// Call	the	ODBC function
				RETCODE nRetCode = ::SQLTables(m_hstmt,
					SQLNTS(pszTableQualifier), SQL_NTS,
					SQLNTS(pszTableOwner), SQL_NTS,
					SQLNTS(pszTableName), SQL_NTS,
					SQLNTS(pszTableType), SQL_NTS);

				if (!Check(nRetCode))
					ThrowDBException(nRetCode, m_hstmt);
				m_lOpen = AFX_RECORDSET_STATUS_OPEN;
			}
			break;
		case SQL_API_SQLPROCEDURES:
			{
				status = initial_catalog;

				LPTSTR pch;
				strSQL = m_strSQL;
				LPCTSTR pszProcQualifier	= pch = strSQL.LockBuffer();
				LPCTSTR pszProcOwner		= pch = LookAheadTerminateSQL(pch);
				LPCTSTR pszProcName			= pch = LookAheadTerminateSQL(pch);
				LookAheadTerminateSQL(pch);

				// Call	the	ODBC function
				RETCODE nRetCode = ::SQLProcedures(m_hstmt,
					SQLNTS(pszProcQualifier), SQL_NTS,
					SQLNTS(pszProcOwner), SQL_NTS,
					SQLNTS(pszProcName), SQL_NTS);

				if (!Check(nRetCode))
					ThrowDBException(nRetCode, m_hstmt);
				m_lOpen = AFX_RECORDSET_STATUS_OPEN;
			}
			break;
		case SQL_API_SQLFOREIGNKEYS:
			{
				status = initial_catalog;

				LPTSTR pch;
				strSQL = m_strSQL;
				LPCTSTR pszPkTableQualifier	= pch = strSQL.LockBuffer();
				LPCTSTR pszPkTableOwner		= pch = LookAheadTerminateSQL(pch);
				LPCTSTR pszPkTableName		= pch = LookAheadTerminateSQL(pch);
				LPCTSTR pszFkTableQualifier	= pch = LookAheadTerminateSQL(pch);
				LPCTSTR pszFkTableOwner		= pch = LookAheadTerminateSQL(pch);
				LPCTSTR pszFkTableName		= pch = LookAheadTerminateSQL(pch);
				LookAheadTerminateSQL(pch);

				// Call	the	ODBC function
				RETCODE nRetCode = ::SQLForeignKeys(m_hstmt,
					SQLNTS(pszPkTableQualifier), SQL_NTS,
					SQLNTS(pszPkTableOwner), SQL_NTS,
					SQLNTS(pszPkTableName), SQL_NTS,
					SQLNTS(pszFkTableQualifier), SQL_NTS,
					SQLNTS(pszFkTableOwner), SQL_NTS,
					SQLNTS(pszFkTableName), SQL_NTS);

				if (!Check(nRetCode))
					ThrowDBException(nRetCode, m_hstmt);
				m_lOpen = AFX_RECORDSET_STATUS_OPEN;
			}
			break;
		case lOpenReadPrimaryKey:
		case SQL_API_SQLPRIMARYKEYS:
			{
				status = initial_catalog;

				LPTSTR pch;
				strSQL = m_strSQL;
				LPCTSTR pszTableQualifier	= pch = strSQL.LockBuffer();
				LPCTSTR pszTableOwner		= pch = LookAheadTerminateSQL(pch);
				LPCTSTR pszTableName		= pch = LookAheadTerminateSQL(pch);
				LookAheadTerminateSQL(pch);

				// Call	the	ODBC function
				RETCODE nRetCode = ::SQLPrimaryKeys(m_hstmt,
					SQLNTS(pszTableQualifier), SQL_NTS,
					SQLNTS(pszTableOwner), SQL_NTS,
					SQLNTS(pszTableName), SQL_NTS);

				if (!Check(nRetCode))
				{
					if (lOpen == SQL_API_SQLPRIMARYKEYS)
						ThrowDBException(nRetCode, m_hstmt);
					lOpen = 0;
				}
				m_lOpen = AFX_RECORDSET_STATUS_OPEN;
				if (lOpen == lOpenReadPrimaryKey)
				{
					AllocAndCacheFieldInfo();
					AllocRowset();
					MoveNext();
					while (!IsEOF())
					{
						CString strColumn;
						GetFieldValue(3, strColumn);
						rgPK.SetAt(strColumn, 0);
						MoveNext();
					}
					Close();
				}
				if (rgPK.IsEmpty())
					m_lOpen = lOpenReadBestRowId;
				else if (lOpen != SQL_API_SQLPRIMARYKEYS)
					BuildSelect(SQLNTS(pszTableOwner), SQLNTS(pszTableName));
			}
			break;
		case SQL_API_SQLTABLEPRIVILEGES:
			{
				status = initial_catalog;

				LPTSTR pch;
				strSQL = m_strSQL;
				LPCTSTR pszTableQualifier	= pch = strSQL.LockBuffer();
				LPCTSTR pszTableOwner		= pch = LookAheadTerminateSQL(pch);
				LPCTSTR pszTableName		= pch = LookAheadTerminateSQL(pch);
				LookAheadTerminateSQL(pch);

				// Call	the	ODBC function
				RETCODE nRetCode = ::SQLTablePrivileges(m_hstmt,
					SQLNTS(pszTableQualifier), SQL_NTS,
					SQLNTS(pszTableOwner), SQL_NTS,
					SQLNTS(pszTableName), SQL_NTS);

				if (!Check(nRetCode))
					ThrowDBException(nRetCode, m_hstmt);
				m_lOpen = AFX_RECORDSET_STATUS_OPEN;
			}
			break;
		case SQL_API_SQLSTATISTICS:
			{
				status = initial_catalog;

				LPTSTR pch;
				strSQL = m_strSQL;
				LPCTSTR pszTableQualifier	= pch = strSQL.LockBuffer();
				LPCTSTR pszTableOwner		= pch = LookAheadTerminateSQL(pch);
				LPCTSTR pszTableName		= pch = LookAheadTerminateSQL(pch);
				LPCTSTR pszUnique			= pch = LookAheadTerminateSQL(pch);
				LPCTSTR pszAccuracy			= pch = LookAheadTerminateSQL(pch);
				LookAheadTerminateSQL(pch);

				// Call	the	ODBC function
				RETCODE nRetCode = ::SQLStatistics(m_hstmt,
					SQLNTS(pszTableQualifier), SQL_NTS,
					SQLNTS(pszTableOwner), SQL_NTS,
					SQLNTS(pszTableName), SQL_NTS,
					SQLNUM(pszUnique, (UWORD)SQL_INDEX_ALL),
					SQLNUM(pszAccuracy, (UWORD)SQL_ENSURE));

				if (!Check(nRetCode))
					ThrowDBException(nRetCode, m_hstmt);
				m_lOpen = AFX_RECORDSET_STATUS_OPEN;
			}
			break;
		case SQL_API_SQLCOLUMNS:
			{
				status = initial_catalog;

				LPTSTR pch;
				strSQL = m_strSQL;
				LPCTSTR pszTableQualifier	= pch = strSQL.LockBuffer();
				LPCTSTR pszTableOwner		= pch = LookAheadTerminateSQL(pch);
				LPCTSTR pszTableName		= pch = LookAheadTerminateSQL(pch);
				LPCTSTR pszColumnName		= pch = LookAheadTerminateSQL(pch);
				LookAheadTerminateSQL(pch);

				// Call	the	ODBC function
				RETCODE nRetCode = ::SQLColumns(m_hstmt,
					SQLNTS(pszTableQualifier), SQL_NTS,
					SQLNTS(pszTableOwner), SQL_NTS,
					SQLNTS(pszTableName), SQL_NTS,
					SQLNTS(pszColumnName), SQL_NTS);

				if (!Check(nRetCode))
					ThrowDBException(nRetCode, m_hstmt);
				m_lOpen = AFX_RECORDSET_STATUS_OPEN;
			}
			break;
		case lOpenReadBestRowId:
		case SQL_API_SQLSPECIALCOLUMNS:
			{
				status = initial_catalog;

				LPTSTR pch;
				strSQL = m_strSQL;
				LPCTSTR pszTableQualifier	= pch = strSQL.LockBuffer();
				LPCTSTR pszTableOwner		= pch = LookAheadTerminateSQL(pch);
				LPCTSTR pszTableName		= pch = LookAheadTerminateSQL(pch);
				LPCTSTR pszType				= pch = LookAheadTerminateSQL(pch);
				LPCTSTR pszScope			= pch = LookAheadTerminateSQL(pch);
				LPCTSTR pszNullable			= pch = LookAheadTerminateSQL(pch);
				LookAheadTerminateSQL(pch);

				// Call	the	ODBC function
				RETCODE nRetCode = ::SQLSpecialColumns(m_hstmt,
					SQLNUM(pszType, SQL_BEST_ROWID),
					SQLNTS(pszTableQualifier), SQL_NTS,
					SQLNTS(pszTableOwner), SQL_NTS,
					SQLNTS(pszTableName), SQL_NTS,
					SQLNUM(pszScope, SQL_SCOPE_CURROW),
					SQLNUM(pszNullable, SQL_NULLABLE));

				if (!Check(nRetCode))
				{
					if (lOpen == SQL_API_SQLSPECIALCOLUMNS)
						ThrowDBException(nRetCode, m_hstmt);
					lOpen = 0;
				}
				m_lOpen = AFX_RECORDSET_STATUS_OPEN;
				if (lOpen == lOpenReadBestRowId)
				{
					AllocAndCacheFieldInfo();
					AllocRowset();
					MoveNext();
					while (!IsEOF())
					{
						CString strColumn;
						GetFieldValue(1, strColumn);
						rgPK.SetAt(strColumn, 0);
						MoveNext();
					}
					Close();
				}
				if (lOpen != SQL_API_SQLSPECIALCOLUMNS)
				{
					BuildSelect(SQLNTS(pszTableOwner), SQLNTS(pszTableName));
				}
			}
			break;
		case SQL_API_SQLPROCEDURECOLUMNS:
			{
				status = initial_catalog;

				LPTSTR pch;
				strSQL = m_strSQL;
				LPCTSTR pszProcQualifier	= pch = strSQL.LockBuffer();
				LPCTSTR pszProcOwner		= pch = LookAheadTerminateSQL(pch);
				LPCTSTR pszProcName			= pch = LookAheadTerminateSQL(pch);
				LPCTSTR pszColumnName		= pch = LookAheadTerminateSQL(pch);
				LookAheadTerminateSQL(pch);

				// Call	the	ODBC function
				RETCODE nRetCode = ::SQLProcedureColumns(m_hstmt,
					SQLNTS(pszProcQualifier), SQL_NTS,
					SQLNTS(pszProcOwner), SQL_NTS,
					SQLNTS(pszProcName), SQL_NTS,
					SQLNTS(pszColumnName), SQL_NTS);

				if (!Check(nRetCode))
					ThrowDBException(nRetCode, m_hstmt);
				m_lOpen = AFX_RECORDSET_STATUS_OPEN;
			}
			break;
		case SQL_API_SQLGETTYPEINFO:
			{
				status = initial_catalog;

				LPTSTR pch;
				strSQL = m_strSQL;
				LPCTSTR pszSqlType			= pch = strSQL.LockBuffer();
				LookAheadTerminateSQL(pch);

				// Call	the	ODBC function
				RETCODE nRetCode = ::SQLGetTypeInfo(m_hstmt,
					SQLNUM(pszSqlType, (UWORD)SQL_ALL_TYPES));

				if (!Check(nRetCode))
					ThrowDBException(nRetCode, m_hstmt);
				m_lOpen = AFX_RECORDSET_STATUS_OPEN;
			}
			break;
		case lOpenView:
			{
				status = initial_catalog;

				LPTSTR pch;
				strSQL = m_strSQL;
				LPCTSTR pszTableQualifier	= pch = strSQL.LockBuffer();
				LPCTSTR pszTableOwner		= pch = LookAheadTerminateSQL(pch);
				LPCTSTR pszTableName		= pch = LookAheadTerminateSQL(pch);
				LookAheadTerminateSQL(pch);

				m_lOpen = 0;
				BuildSelect(SQLNTS(pszTableOwner), SQLNTS(pszTableName));
			}
			break;
		case SQL_API_SQLEXECDIRECT:
			status = initial_sql;
			// fall through
		default:
			if (int cchPrefix = IsPrefix(m_strSQL, _T("SP_HELPTEXT ")))
			{
				status = initial_sp_helptext;
				if (static_cast<CDatabaseEx *>(m_pDatabase)->PoweredBy("ORACLE"))
				{
					CString sType;
					AfxExtractSubString(sType, m_strSQL, 1, '\'');
					CString sMajor, sMinor, sConjxn;
					AfxExtractSubString(sMajor, sType, 0, '.');
					AfxExtractSubString(sMinor, sType, 1, '.');
					if (sMajor.CompareNoCase(_T("PUBLIC")))
						sConjxn.Format(_T(" AND OWNER = '%s'"), (LPCTSTR)sMajor);
					sType.Format(
						_T("SELECT OBJECT_TYPE FROM ALL_OBJECTS WHERE OBJECT_NAME = '%s'%s"),
						(LPCTSTR)sMinor, (LPCTSTR)sConjxn);
					CRecordset rs = m_pDatabase;
					rs.Open(forwardOnly, sType, executeDirect);
					if (!rs.IsEOF())
					{
						rs.GetFieldValue((short)0, sType);
						m_strSQL.Format(
							sType.CompareNoCase(_T("VIEW")) == 0 ?
							_T("SELECT TEXT FROM ALL_VIEWS WHERE VIEW_NAME = '%s'%s") :
							_T("SELECT TEXT FROM ALL_SOURCE WHERE NAME = '%s'%s"),
							(LPCTSTR)sMinor, (LPCTSTR)sConjxn);
					}
				}
			}
			else if (int cchPrefix = IsPrefix(m_strSQL, _T("USE ")))
			{
				status = initial_use;
			}
			if (m_strFilter.GetLength())
				m_strSQL += _T(" WHERE ") + m_strFilter;
			ReplaceBrackets(m_strSQL);
			PrepareAndExecute();
			break;
		}
	} while (m_lOpen != AFX_RECORDSET_STATUS_OPEN);
	AllocAndCacheFieldInfo();
	m_bUpdatable = FALSE;
	short n = 0;
	while (n < m_nResultCols)
	{
		CODBCFieldInfo &fieldInfo = m_rgODBCFieldInfos[n++];
		fieldInfo.m_nNullability &= ~0xF000;
		void *pv = 0;
		if (rgPK.Lookup(fieldInfo.m_strName, pv))
		{
			fieldInfo.m_nNullability |= nNullabilityPK;
			m_bUpdatable = TRUE;
		}
		SQLINTEGER f = SQL_FALSE;
		SQLColAttribute(m_hstmt, n, SQL_COLUMN_AUTO_INCREMENT, 0, 0, 0, (SQLPOINTER)&f);
		if (f)
			fieldInfo.m_nNullability |= nNullabilityAutoIncrement;
		f = SQL_FALSE;
		SQLColAttribute(m_hstmt, n, SQL_COLUMN_UPDATABLE, 0, 0, 0, (SQLPOINTER)&f);
		if (f)
			fieldInfo.m_nNullability |= nNullabilityUpdatable;
	}
	AllocRowset();
	if (m_nResultCols == 0)
		return status;
	MoveNext();
	return status;
}

BOOL CRecordsetEx::MoreResults()
{
	if (!FlushResultSet())
		return FALSE;
	m_nResultCols = -1;
	delete[] m_rgODBCFieldInfos;
	m_rgODBCFieldInfos = NULL;
	AllocAndCacheFieldInfo();
	MoveNext();
	return TRUE;
}

void CRecordsetEx::Release()
{
	Close();
	delete this;
}
