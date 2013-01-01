/*/ODBCTrace/Legend.cpp

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
#include "Legend.h"

Legend::Legend(short &r, const char *(*callback)(int))
{
	const char *s = callback ? callback(r) : 0;
	wsprintf(text, s ? FMT_HX " = %s" : FMT_HX, r, s);
	reinterpret_cast<const char *&>(r) = text;
}

Legend::Legend(short *r, const char *(*callback)(int))
{
	if (r)
	{
		const char *s = callback ? callback(*r) : 0;
		wsprintf(text, s ? FMT_HX " = %s" : FMT_HX, *r, s);
	}
	else
	{
		lstrcpy(text, "(null)");
	}
}

Legend::Legend(unsigned short &r, const char *(*callback)(int))
{
	const char *s = callback ? callback(r) : 0;
	wsprintf(text, s ? FMT_HX " = %s" : FMT_HX, r, s);
	reinterpret_cast<const char *&>(r) = text;
}

Legend::Legend(unsigned short *r, const char *(*callback)(int))
{
	if (r)
	{
		const char *s = callback ? callback(*r) : 0;
		wsprintf(text, s ? FMT_HX " = %s" : FMT_HX, *r, s);
	}
	else
	{
		lstrcpy(text, "(null)");
	}
}

Legend::Legend(long &r, const char *(*callback)(int))
{
	const char *s = callback ? callback(r) : 0;
	wsprintf(text, s ? FMT_LX " = %s" : FMT_LX, r, s);
	reinterpret_cast<const char *&>(r) = text;
}

Legend::Legend(long *r, const char *(*callback)(int))
{
	if (r)
	{
		const char *s = callback ? callback(*r) : 0;
		wsprintf(text, s ? FMT_LX " = %s" : FMT_LX, *r, s);
	}
	else
	{
		lstrcpy(text, "(null)");
	}
}

Legend::Legend(unsigned long &r, const char *(*callback)(int))
{
	const char *s = callback ? callback(r) : 0;
	wsprintf(text, s ? FMT_LX " = %s" : FMT_LX, r, s);
	reinterpret_cast<const char *&>(r) = text;
}

Legend::Legend(unsigned long *r, const char *(*callback)(int))
{
	if (r)
	{
		const char *s = callback ? callback(*r) : 0;
		wsprintf(text, s ? FMT_LX " = %s" : FMT_LX, *r, s);
	}
	else
	{
		lstrcpy(text, "(null)");
	}
}

#define L(L) C_ASSERT(sizeof "0x00000000 = " #L <= sizeof((Legend*)0)->text); case L: return #L;

const char *Legend::SqlType(int n)
{
	switch (n)
	{
		L(SQL_CHAR)
		L(SQL_NUMERIC)
		L(SQL_DECIMAL)
		L(SQL_INTEGER)
		L(SQL_SMALLINT)
		L(SQL_FLOAT)
		L(SQL_REAL)
		L(SQL_DOUBLE)
		L(SQL_DATE)
		L(SQL_TIME)
		L(SQL_TIMESTAMP)
		L(SQL_VARCHAR)
		L(SQL_TYPE_DATE)
		L(SQL_TYPE_TIME)
		L(SQL_TYPE_TIMESTAMP)
		L(SQL_INTERVAL_YEAR)
		L(SQL_INTERVAL_MONTH)
		L(SQL_INTERVAL_DAY)
		L(SQL_INTERVAL_HOUR)
		L(SQL_INTERVAL_MINUTE)
		L(SQL_INTERVAL_SECOND)
		L(SQL_INTERVAL_YEAR_TO_MONTH)
		L(SQL_INTERVAL_DAY_TO_HOUR)
		L(SQL_INTERVAL_DAY_TO_MINUTE)
		L(SQL_INTERVAL_DAY_TO_SECOND)
		L(SQL_INTERVAL_HOUR_TO_MINUTE)
		L(SQL_INTERVAL_HOUR_TO_SECOND)
		L(SQL_INTERVAL_MINUTE_TO_SECOND)
		L(SQL_LONGVARCHAR)
		L(SQL_BINARY)
		L(SQL_VARBINARY)
		L(SQL_LONGVARBINARY)
		L(SQL_BIGINT)
		L(SQL_TINYINT)
		L(SQL_BIT)
		L(SQL_WCHAR)
		L(SQL_WVARCHAR)
		L(SQL_WLONGVARCHAR)
		L(SQL_GUID)
	}
	return 0;
}

const char *Legend::SqlCType(int n)
{
	switch (n)
	{
		L(SQL_C_CHAR)
		L(SQL_C_NUMERIC)
		L(SQL_C_LONG)
		L(SQL_C_SHORT)
		L(SQL_C_FLOAT)
		L(SQL_C_DOUBLE)
		L(SQL_C_DATE)
		L(SQL_C_TYPE_DATE)
		L(SQL_C_TYPE_TIME)
		L(SQL_C_TYPE_TIMESTAMP)
		L(SQL_C_INTERVAL_YEAR)
		L(SQL_C_INTERVAL_MONTH)
		L(SQL_C_INTERVAL_DAY)
		L(SQL_C_INTERVAL_HOUR)
		L(SQL_C_INTERVAL_MINUTE)
		L(SQL_C_INTERVAL_SECOND)
		L(SQL_C_INTERVAL_YEAR_TO_MONTH)
		L(SQL_C_INTERVAL_DAY_TO_HOUR)
		L(SQL_C_INTERVAL_DAY_TO_MINUTE)
		L(SQL_C_INTERVAL_DAY_TO_SECOND)
		L(SQL_C_INTERVAL_HOUR_TO_MINUTE)
		L(SQL_C_INTERVAL_HOUR_TO_SECOND)
		L(SQL_C_INTERVAL_MINUTE_TO_SECOND)
		L(SQL_C_BINARY)
		L(SQL_C_TINYINT)
		L(SQL_C_BIT)
		L(SQL_C_GUID)
		L(SQL_C_SBIGINT)
		L(SQL_C_UBIGINT)
		L(SQL_C_SLONG)
		L(SQL_C_ULONG)
		L(SQL_C_SSHORT)
		L(SQL_C_USHORT)
		L(SQL_C_STINYINT)
		L(SQL_C_UTINYINT)
	}
	return 0;
}

const char *Legend::SqlDriverCompletion(int n)
{
	switch (n)
	{
		L(SQL_DRIVER_NOPROMPT)
		L(SQL_DRIVER_COMPLETE)
		L(SQL_DRIVER_PROMPT)
		L(SQL_DRIVER_COMPLETE_REQUIRED)
	}
	return 0;
}

const char *Legend::SqlCompletionType(int n)
{
	switch (n)
	{
		L(SQL_COMMIT)
		L(SQL_ROLLBACK)
	}
	return 0;
}

const char *Legend::SqlStmtAttr(int n)
{
	switch (n)
	{
		L(SQL_ATTR_APP_PARAM_DESC)
		L(SQL_ATTR_APP_ROW_DESC)
		L(SQL_ATTR_ASYNC_ENABLE)
		L(SQL_ATTR_CONCURRENCY)
		L(SQL_ATTR_CURSOR_SCROLLABLE)
		L(SQL_ATTR_CURSOR_SENSITIVITY)
		L(SQL_ATTR_CURSOR_TYPE)
		L(SQL_ATTR_ENABLE_AUTO_IPD)
		L(SQL_ATTR_FETCH_BOOKMARK_PTR)
		L(SQL_ATTR_IMP_PARAM_DESC)
		L(SQL_ATTR_IMP_ROW_DESC)
		L(SQL_ATTR_KEYSET_SIZE)
		L(SQL_ATTR_MAX_LENGTH)
		L(SQL_ATTR_MAX_ROWS)
		L(SQL_ATTR_METADATA_ID)
		L(SQL_ATTR_NOSCAN)
		L(SQL_ATTR_PARAM_BIND_OFFSET_PTR)
		L(SQL_ATTR_PARAM_BIND_TYPE)
		L(SQL_ATTR_PARAM_OPERATION_PTR)
		L(SQL_ATTR_PARAM_STATUS_PTR)
		L(SQL_ATTR_PARAMS_PROCESSED_PTR)
		L(SQL_ATTR_PARAMSET_SIZE)
		L(SQL_ATTR_QUERY_TIMEOUT)
		L(SQL_ATTR_RETRIEVE_DATA)
		L(SQL_ATTR_ROW_ARRAY_SIZE)
		L(SQL_ATTR_ROW_BIND_OFFSET_PTR)
		L(SQL_ATTR_ROW_BIND_TYPE)
		L(SQL_ATTR_ROW_NUMBER)
		L(SQL_ATTR_ROW_OPERATION_PTR)
		L(SQL_ATTR_ROW_STATUS_PTR)
		L(SQL_ATTR_ROWS_FETCHED_PTR)
		L(SQL_ATTR_SIMULATE_CURSOR)
		L(SQL_ATTR_USE_BOOKMARKS)
	}
	return 0;
}

const char *Legend::SqlStmtOption(int n)
{
	switch (n)
	{
		L(SQL_QUERY_TIMEOUT)
		L(SQL_MAX_ROWS)
		L(SQL_NOSCAN)
		L(SQL_MAX_LENGTH)
		L(SQL_ASYNC_ENABLE)
		L(SQL_BIND_TYPE)
		L(SQL_CURSOR_TYPE)
		L(SQL_CONCURRENCY)
		L(SQL_KEYSET_SIZE)
		L(SQL_ROWSET_SIZE)
		L(SQL_SIMULATE_CURSOR)
		L(SQL_RETRIEVE_DATA)
		L(SQL_USE_BOOKMARKS)
		L(SQL_GET_BOOKMARK)			// GetStmtOption Only
		L(SQL_ROW_NUMBER)			// GetStmtOption Only
	}
	return 0;
}

const char *Legend::SqlConnectAttr(int n)
{
	switch (n)
	{
		L(SQL_ATTR_ACCESS_MODE)
		L(SQL_ATTR_ASYNC_ENABLE)
		L(SQL_ATTR_AUTO_IPD)
		L(SQL_ATTR_AUTOCOMMIT)
		L(SQL_ATTR_CONNECTION_DEAD)
		L(SQL_ATTR_CONNECTION_TIMEOUT)
		L(SQL_ATTR_CURRENT_CATALOG)
		L(SQL_ATTR_LOGIN_TIMEOUT)
		L(SQL_ATTR_METADATA_ID)
		L(SQL_ATTR_ODBC_CURSORS)
		L(SQL_ATTR_PACKET_SIZE)
		L(SQL_ATTR_QUIET_MODE)
		L(SQL_ATTR_TRACE)
		L(SQL_ATTR_TRACEFILE)
		L(SQL_ATTR_TRANSLATE_LIB)
		L(SQL_ATTR_TRANSLATE_OPTION)
		L(SQL_ATTR_TXN_ISOLATION)
	}
	return 0;
}

const char *Legend::SqlConnectOption(int n)
{
	switch (n)
	{
		L(SQL_ACCESS_MODE)
		L(SQL_AUTOCOMMIT)
		L(SQL_CURRENT_QUALIFIER)
		L(SQL_LOGIN_TIMEOUT)
		L(SQL_ODBC_CURSORS)
		L(SQL_PACKET_SIZE)
		L(SQL_QUIET_MODE)
		L(SQL_OPT_TRACE)
		L(SQL_OPT_TRACEFILE)
		L(SQL_TRANSLATE_DLL)
		L(SQL_TRANSLATE_OPTION)
		L(SQL_TXN_ISOLATION)
	}
	return 0;
}

const char *Legend::SqlFreeStmtOption(int n)
{
	switch (n)
	{
		L(SQL_CLOSE)
		L(SQL_DROP)
		L(SQL_UNBIND)
		L(SQL_RESET_PARAMS)
	}
	return 0;
}

const char *Legend::SqlSetPosOperation(int n)
{
	switch (n)
	{
		L(SQL_POSITION)
		L(SQL_REFRESH)
		L(SQL_UPDATE)
		L(SQL_DELETE)
	}
	return 0;
}

const char *Legend::SqlSetPosLockType(int n)
{
	switch (n)
	{
		L(SQL_LOCK_NO_CHANGE)
		L(SQL_LOCK_EXCLUSIVE)
		L(SQL_LOCK_UNLOCK)
	}
	return 0;
}

const char *Legend::SqlInputOutputType(int n)
{
	switch (n)
	{
		L(SQL_PARAM_TYPE_UNKNOWN)
		L(SQL_PARAM_INPUT)
		L(SQL_PARAM_INPUT_OUTPUT)
		L(SQL_RESULT_COL)
		L(SQL_PARAM_OUTPUT)
		L(SQL_RETURN_VALUE)
	}
	return 0;
}

const char *Legend::SqlAttrOdbcCursors(int n)
{
	switch (n)
	{
		L(SQL_CUR_USE_IF_NEEDED)
		L(SQL_CUR_USE_ODBC)
		L(SQL_CUR_USE_DRIVER)
	}
	return 0;
}
