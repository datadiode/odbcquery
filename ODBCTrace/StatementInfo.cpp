/*/ODBCTrace/TraceWnd.cpp

Copyright (c) 2006 Jochen Neubeck

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
#include "stdafx.h"
#include "TraceWnd.h"

StatementInfo *StatementInfo::pHead = 0;

void StatementInfo::Insert(StatementInfo *pItem)
{
	if (pHead)
	{
		pItem->Blink = pHead;
		pItem->Flink = pHead->Flink;
		pHead->Flink = pHead->Flink->Blink = pItem;
	}
	else
	{
		pItem->Blink = pItem->Flink = pItem;
	}
	pHead = pItem;
}

void StatementInfo::Delete(StatementInfo *pItem)
{
	LIST_ENTRY *pTail = pHead->Flink;
	pItem->Blink->Flink = pItem->Flink;
	pItem->Flink->Blink = pItem->Blink;
	pItem->Flink = pItem->Blink = pItem;
	if (pHead == pItem)
	{
		pHead = pTail != pHead ? static_cast<StatementInfo *>(pTail) : 0;
	}
}

StatementInfo *StatementInfo::FromHandle(SQLHSTMT StatementHandle, UINT nParams, UINT nColumns)
{
	StatementInfo *pItem = pHead;
	while (pItem && pItem->StatementHandle != StatementHandle)
	{
		pItem = static_cast<StatementInfo *>(pItem->Flink);
		if (pItem == pHead)
			pItem = 0;
	}
	UINT cbTrunk = 0;
	if (pItem)
	{
		cbTrunk = FIELD_OFFSET(StatementInfo, rgParams[pItem->nParams + pItem->nColumns]);
		Delete(pItem);
		if (nParams < pItem->nParams)
			nParams = pItem->nParams;
		if (nColumns < pItem->nColumns)
			nColumns = pItem->nColumns;
	}
	UINT cbTotal = FIELD_OFFSET(StatementInfo, rgParams[nParams + nColumns]);
	pItem = static_cast<StatementInfo *>(CoTaskMemRealloc(pItem, cbTotal));
	ZeroMemory(LPBYTE(pItem) + cbTrunk, cbTotal - cbTrunk);
	pItem->StatementHandle = StatementHandle;
	if (pItem->nColumns < nColumns)
		pItem->nColumns = nColumns;
	if (pItem->nParams < nParams)
	{
		while (nColumns)
		{
			--nColumns;
			pItem->rgParams[nParams + nColumns] = pItem->rgParams[pItem->nParams + nColumns];
		}
		pItem->nParams = nParams;
	}
	Insert(pItem);
	return pItem;
}

void StatementInfo::SetStatementText(LPCSTR pszSrc, int cchSrc)
{
	if (cchSrc == SQL_NTS)
		cchSrc = lstrlenA(pszSrc);
	int cbAlloc = cchSrc + 3;
	LPSTR pszDst = StatementText = (LPSTR)CoTaskMemRealloc(StatementText, cbAlloc);
	*pszDst = '\t';
	static const CHAR szSpace[] = "\t\r\n\40";
	while ((cchSrc = StrCSpnA(pszSrc += StrSpnA(pszSrc, szSpace), szSpace)) != 0)
	{
		lstrcpyn(++pszDst, pszSrc, cchSrc + 1);
		pszDst += cchSrc;
		*pszDst = '\40';
		pszSrc += cchSrc;
	}
	pszDst[0] = '\n';
	pszDst[1] = '\0';
}

void StatementInfo::SetStatementText(LPCWSTR pszSrc, int cchSrc)
{
	if (cchSrc == SQL_NTS)
		cchSrc = -1;
	int cbAlloc = WideCharToMultiByte(CP_ACP, 0, pszSrc, cchSrc, 0, 0, 0, 0) + 3;
	LPSTR pszDst = StatementText = (LPSTR)CoTaskMemRealloc(StatementText, cbAlloc);
	*pszDst = '\t';
	static const WCHAR szSpace[] = L"\t\r\n\40";
	while ((cchSrc = StrCSpnW(pszSrc += StrSpnW(pszSrc, szSpace), szSpace)) != 0)
	{
		int cchDst = WideCharToMultiByte(CP_ACP, 0, pszSrc, cchSrc, ++pszDst, --cbAlloc, 0, 0);
		pszDst += cchDst;
		cbAlloc -= cchDst;
		*pszDst = '\40';
		pszSrc += cchSrc;
	}
	pszDst[0] = '\n';
	pszDst[1] = '\0';
}

StatementInfo::Param *StatementInfo::GetParam(UINT iParam)
{
	return iParam > 0 && iParam <= nParams ? rgParams + iParam - 1 : 0;
}

StatementInfo::Param *StatementInfo::GetColumn(UINT iColumn)
{
	return iColumn > 0 && iColumn <= nColumns ? rgParams + nParams + iColumn - 1 : 0;
}

STDMETHODIMP StatementInfo::WriteLiteral(ISequentialStream *piStream, const Param *pParam)
{
	ULONG cbSrcLength = pParam->BufferLength ? pParam->BufferLength : pParam->ColumnSize;
	DBSTATUS dbsSrcStatus = DBSTATUS_S_OK;
	DBSTATUS dbsDstStatus = DBSTATUS_S_OK;
	BYTE bPrecision = 0;
	BYTE bScale = 0;
	DBTYPE wSrcType = DBTYPE_EMPTY;
	DBDATACONVERT flags = DBDATACONVERT_DEFAULT;
	int nEatQuotes = 1;
	int nEatBraces = 0;
	TCHAR szDstValue[1024 + 6 + 2 + 4];
	LPTSTR pchText = szDstValue + 6;
	*LPDWORD(szDstValue + 0) = '{{{{';
	*LPDWORD(szDstValue + 4) = 'd  d';
	if (pParam->StrLen_or_IndPtr)
	{
		switch (*pParam->StrLen_or_IndPtr)
		{
		case SQL_NULL_DATA:
			dbsSrcStatus = DBSTATUS_S_ISNULL;
			break;
		case SQL_DATA_AT_EXEC:
			dbsSrcStatus = DBSTATUS_E_UNAVAILABLE;
			break;
		case SQL_NTS:
			cbSrcLength = 0;
			flags = DBDATACONVERT_LENGTHFROMNTS;
			break;
		case -6:
			dbsSrcStatus = DBSTATUS_S_DEFAULT;
			break;
		default:
			if (cbSrcLength > ULONG(*pParam->StrLen_or_IndPtr))
				cbSrcLength = ULONG(*pParam->StrLen_or_IndPtr);
			break;
		}
	}
	else
	{
		flags = DBDATACONVERT_LENGTHFROMNTS;
	}
	switch (pParam->ParameterType)
	{
	case SQL_CHAR:
	case SQL_VARCHAR:
	case SQL_LONGVARCHAR:
	case SQL_BINARY:
	case SQL_VARBINARY:
	case SQL_LONGVARBINARY:
	case SQL_WCHAR:
	case SQL_WVARCHAR:
	case SQL_WLONGVARCHAR:
		nEatQuotes = 0;
		break;
	}
	switch (pParam->ValueType)
	{
	case SQL_C_WCHAR:
		wSrcType = DBTYPE_WSTR;
		break;
	case SQL_C_CHAR:
		wSrcType = DBTYPE_STR;
		break;
	case SQL_C_BINARY:
		wSrcType = DBTYPE_BYTES;
		break;
	case SQL_C_DATE:
	case SQL_C_TYPE_DATE:
		wSrcType = DBTYPE_DBDATE;
		nEatQuotes = -1;
		pchText = szDstValue + 4;
		szDstValue[4] = 'd';
		break;
	case SQL_C_TIME:
	case SQL_C_TYPE_TIME:
		wSrcType = DBTYPE_DBTIME;
		nEatQuotes = -1;
		pchText = szDstValue + 4;
		szDstValue[4] = 't';
		break;
	case SQL_C_TIMESTAMP:
	case SQL_C_TYPE_TIMESTAMP:
		wSrcType = DBTYPE_DBTIMESTAMP;
		nEatQuotes = -1;
		pchText = szDstValue + 3;
		szDstValue[3] = 't';
		szDstValue[4] = 's';
		break;
	case SQL_C_LONG:
	case SQL_C_SLONG:
		wSrcType = DBTYPE_I4;
		break;
	case SQL_C_ULONG:
		wSrcType = DBTYPE_UI4;
		break;
	case SQL_C_SHORT:
	case SQL_C_SSHORT:
		wSrcType = DBTYPE_I2;
		break;
	case SQL_C_USHORT:
		wSrcType = DBTYPE_UI2;
	case SQL_C_TINYINT:
	case SQL_C_STINYINT:
		wSrcType = DBTYPE_I1;
		break;
	case SQL_C_UTINYINT:
		wSrcType = DBTYPE_UI1;
		break;
	case SQL_BIGINT:
	case SQL_C_SBIGINT:
		wSrcType = DBTYPE_I8;
		break;
	case SQL_C_UBIGINT:
		wSrcType = DBTYPE_UI8;
		break;
	case SQL_C_FLOAT:
		wSrcType = DBTYPE_R4;
		break;
	case SQL_C_DOUBLE:
		wSrcType = DBTYPE_R8;
		break;
	case SQL_C_GUID:
		wSrcType = DBTYPE_GUID;
		nEatQuotes = -1;
		nEatBraces = 1;
		pchText = szDstValue + 1;
		szDstValue[1] = 'g';
		szDstValue[2] = 'u';
		szDstValue[3] = 'i';
		szDstValue[4] = 'd';
		break;
	case SQL_C_NUMERIC:
		wSrcType = DBTYPE_NUMERIC;
	default:
		bPrecision = (BYTE)pParam->ColumnSize;
		bScale = (BYTE)pParam->DecimalDigits;
		break;
	}
	ULONG cbDstLength = sizeof szDstValue - 2 - 6 - 4;
	HRESULT hr = ODBCTrace.pDataConvert->DataConvert
	(
		wSrcType,
		DBTYPE_STR,
		cbSrcLength,
		&cbDstLength,
		pParam->ParameterValuePtr,
		szDstValue + 1 + 6 - nEatBraces,
		sizeof szDstValue - 2 - 6 - 4,
		dbsSrcStatus,
		&dbsDstStatus,
		bPrecision,
		bScale,
		flags
	);
	LPCTSTR pszErrFmt = TEXT("ERR#%lX");
	if SUCCEEDED(hr)
	{
		cbDstLength -= 2 * nEatBraces;
		switch (hr = dbsDstStatus)
		{
		case DBSTATUS_S_ISNULL:
			pszErrFmt = TEXT("NULL");
			break;
		case DBSTATUS_S_DEFAULT:
			pszErrFmt = TEXT("{DEFAULT}");
			break;
		case DBSTATUS_S_IGNORE:
			pszErrFmt = TEXT("{IGNORE}");
			break;
		case DBSTATUS_S_TRUNCATED:
			if (cbDstLength >= sizeof szDstValue - 2 - 6 - 4)
			{
				cbDstLength = sizeof szDstValue - 2 - 6 - 4;
				*LPDWORD(szDstValue + sizeof szDstValue - 8) = '....';
			}
			hr = 0;
			break;
		}
	}
	if (hr)
	{
		pchText = szDstValue + 6;
		cbDstLength = wsprintf(szDstValue + 1 + 6, pszErrFmt, hr);
		nEatQuotes = 1;
	}
	szDstValue[cbDstLength + 6 + 1] = '\'';
	szDstValue[cbDstLength + 6 + 2] = '}';
	szDstValue[cbDstLength + 6 + 3] = '\0';
	while (LPTSTR pchQuote = StrChr(pchText + 1, '\''))
	{
		szDstValue[6] = '\'';
		if FAILED(hr = piStream->Write(pchText + nEatQuotes, pchQuote + 1 - pchText - 2 * nEatQuotes, 0))
			return hr;
		pchText = pchQuote;
	}
	return S_OK;
}

static LPCTSTR NTAPI FindMarker(LPCTSTR pc)
{
	int q = 0;
recurse:
	switch (int c = *pc)
	{
	case '\0':
		return 0;
	case '\'':
	case '\"':
		if (q == 0 || q == c)
			q ^= c;
		break;
	case '?':
		if (q == 0)
			return pc;
		break;
	}
	++pc;
	goto recurse;
}

STDMETHODIMP StatementInfo::WriteSQL(ISequentialStream *piStream, BOOL bTerminate)
{
	UINT iParam = 0;
	LPCTSTR fmt = StatementText;
	HRESULT hr;
	while (LPCTSTR pct = FindMarker(fmt))
	{
		if FAILED(hr = piStream->Write(fmt, (pct - fmt) * sizeof(TCHAR), 0))
			return hr;
		if (Param *pParam = GetParam(++iParam))
			if FAILED(hr = WriteLiteral(piStream, pParam))
				return hr;
		fmt = pct + 1;
	}
	int cchText = lstrlen(fmt);
	if (bTerminate)
		++cchText;
	return piStream->Write(fmt, cchText * sizeof(TCHAR), 0);
}

LPTSTR StatementInfo::BuildSQL(SQLHSTMT StatementHandle)
{
	LPVOID pMem = 0;
	if (StatementInfo *pInfo = FromHandle(StatementHandle))
	{
		IStream *piStream = 0;
		if SUCCEEDED(CreateStreamOnHGlobal(0, FALSE, &piStream))
		{
			pInfo->WriteSQL(piStream, TRUE);
			HGLOBAL hMem = 0;
			if SUCCEEDED(GetHGlobalFromStream(piStream, &hMem))
			{
				pMem = GlobalLock(hMem);
				if (!pMem)
					GlobalFree(hMem);
			}
			piStream->Release();
		}
	}
	return static_cast<LPTSTR>(pMem);
	// caller must GlobalFreePtr(pMem)
}

#define SQL_FAILED(rc) (!SQL_SUCCEEDED(rc))

STDMETHODIMP StatementInfo::DumpColumns(ISequentialStream *piStream, SQLINTEGER iRow, BOOL bTerminate)
{
	HRESULT hr = 0;
	SQLSMALLINT nCols = 0;
	if SQL_FAILED(SQLNumResultCols(StatementHandle, &nCols))
		return E_FAIL;
	SQLUINTEGER cbRowOffset = 0;
	SQLINTEGER *pRowOffset = 0;
	if SQL_FAILED(SQLGetStmtAttr(StatementHandle, SQL_ATTR_ROW_BIND_OFFSET_PTR, &pRowOffset, SQL_IS_POINTER, 0))
		return E_FAIL;
	if (pRowOffset)
		cbRowOffset = *pRowOffset;
	SQLUINTEGER cbBindType = 0;
	if SQL_FAILED(SQLGetStmtAttr(StatementHandle, SQL_ATTR_ROW_BIND_TYPE, &cbBindType, SQL_IS_UINTEGER, 0))
		return E_FAIL;
	--iRow;
	for (SQLSMALLINT iCol = 1 ; iCol <= nCols ; ++iCol)
	{
		SQLCHAR szName[80];
		Param param;
		SQLSMALLINT cchName = 0;
		SQLSMALLINT Nullable;
		if SQL_FAILED(SQLDescribeCol(StatementHandle, iCol, szName + 1, sizeof szName - 2, &cchName, &param.ParameterType, &param.ColumnSize, &param.DecimalDigits, &Nullable))
			cchName = GetAtomNameA(iCol, (LPSTR)szName + 1, sizeof szName - 2);
		szName[0] = szName[++cchName] = '\t';
		if FAILED(hr = piStream->Write(szName, ++cchName, 0))
			return hr;
		if (Param *pParam = GetColumn(iCol))
		{
			// Bound Address + Binding Offset + ((Row Number - 1) x Element Size)
			param.ValueType			= pParam->ValueType;
			param.BufferLength		= pParam->BufferLength;
			param.ParameterValuePtr	= 0;
			param.StrLen_or_IndPtr	= 0;
			if (cbBindType)
			{
				if (pParam->ParameterValuePtr)
					param.ParameterValuePtr	= (LPBYTE)pParam->ParameterValuePtr + cbRowOffset + iRow * cbBindType;
				if (pParam->StrLen_or_IndPtr)
					(LPBYTE&)param.StrLen_or_IndPtr	= (LPBYTE)pParam->StrLen_or_IndPtr + cbRowOffset + iRow * cbBindType;
			}
			else
			{
				if (pParam->ParameterValuePtr)
					param.ParameterValuePtr	= (LPBYTE)pParam->ParameterValuePtr + iRow * pParam->BufferLength;
				if (pParam->StrLen_or_IndPtr)
					(LPBYTE&)param.StrLen_or_IndPtr	= (LPBYTE)pParam->StrLen_or_IndPtr + iRow * sizeof(SQLINTEGER);
			}
			if FAILED(hr = WriteLiteral(piStream, &param))
				return hr;
		}
		if FAILED(hr = piStream->Write("\r\n", bTerminate && iCol == nCols ? 3 : 2, 0))
			return hr;
	}
	return S_OK;
}

LPTSTR StatementInfo::ColumnDump(SQLHSTMT StatementHandle, const SQLUINTEGER nRows)
{
	SQLUINTEGER nHead = nRows < ODBCTrace.nLimitRowDump ? nRows : ODBCTrace.nLimitRowDump;
	LPVOID pMem = 0;
	if (StatementInfo *pInfo = FromHandle(StatementHandle))
	{
		IStream *piStream = 0;
		if SUCCEEDED(CreateStreamOnHGlobal(0, FALSE, &piStream))
		{
 			for (SQLUINTEGER iRow = 1 ; iRow <= nHead ; ++iRow)
			{
				TCHAR szText[20];
				piStream->Write(szText, wsprintf(szText, TEXT("\t[%lu]\r\n"), iRow), 0);
				pInfo->DumpColumns(piStream, iRow, iRow == nRows);
			}
			if (nRows > ODBCTrace.nLimitRowDump)
			{
				TCHAR szText[] = TEXT("\t[...]\r\n");
				piStream->Write(szText, sizeof szText, 0);
			}
			HGLOBAL hMem = 0;
			if SUCCEEDED(GetHGlobalFromStream(piStream, &hMem))
			{
				pMem = GlobalLock(hMem);
				if (!pMem)
					GlobalFree(hMem);
			}
			piStream->Release();
		}
	}
	return static_cast<LPTSTR>(pMem);
	// caller must GlobalFreePtr(pMem)
}

// Dump an unbound column
STDMETHODIMP StatementInfo::DumpUnboundColumn(ISequentialStream *piStream, Param *pParam, SQLHSTMT StatementHandle, SQLSMALLINT iCol, BOOL bTerminate)
{
	HRESULT hr = 0;
	SQLCHAR szName[80];
	SQLSMALLINT cchName = 0;
	SQLSMALLINT Nullable;
	if SQL_FAILED(SQLDescribeCol(StatementHandle, iCol, szName + 1, sizeof szName - 2, &cchName, &pParam->ParameterType, &pParam->ColumnSize, &pParam->DecimalDigits, &Nullable))
		cchName = GetAtomNameA(iCol, (LPSTR)szName + 1, sizeof szName - 2);
	szName[0] = szName[++cchName] = '\t';
	if FAILED(hr = piStream->Write(szName, ++cchName, 0))
		return hr;
	if FAILED(hr = WriteLiteral(piStream, pParam))
		return hr;
	return piStream->Write("\r\n", bTerminate ? 3 : 2, 0);
}

LPTSTR StatementInfo::UnboundColumnDump(SQLHSTMT StatementHandle, SQLSMALLINT ColumnNumber, SQLSMALLINT TargetType, SQLPOINTER TargetValuePtr, SQLINTEGER BufferLength, SQLINTEGER *StrLen_or_IndPtr)
{
	LPVOID pMem = 0;
	IStream *piStream = 0;
	if SUCCEEDED(CreateStreamOnHGlobal(0, FALSE, &piStream))
	{
		Param param;
		param.ValueType			= TargetType;
		param.ParameterValuePtr	= TargetValuePtr;
		param.BufferLength		= BufferLength;
		param.StrLen_or_IndPtr	= StrLen_or_IndPtr;
		DumpUnboundColumn(piStream, &param, StatementHandle, ColumnNumber, TRUE);
		HGLOBAL hMem = 0;
		if SUCCEEDED(GetHGlobalFromStream(piStream, &hMem))
		{
			pMem = GlobalLock(hMem);
			if (!pMem)
				GlobalFree(hMem);
		}
		piStream->Release();
	}
	return static_cast<LPTSTR>(pMem);
	// caller must GlobalFreePtr(pMem)
}
