/*/ODBCTrace/ODBCTrace.cpp

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
#include "TraceWnd.h"

#define fXX(name, type) union { type name; const char *_##name; };
#define fPX(name, type) "\t" #name "\t" #type "\t" FMT_PX "\n"
#define fHX(name, type) "\t" #name "\t" #type "\t" FMT_HX "\n"
#define fGX(name, type) "\t" #name "\t" #type "\t" FMT_GX "\n"
#define fLX(name, type) "\t" #name "\t" #type "\t" FMT_LX "\n"
#define fHS(name, type) "\t" #name "\t" #type "\t" FMT_HS "\n"
#define fLS(name, type) "\t" #name "\t" #type "\t" FMT_LS "\n"

#define fHA fHS
#define fLA fLS

class CallFrameSQLTables : public CallFrame
{
public:
	struct Args
	{
		fXX(StatementHandle,		SQLHSTMT)
		fXX(CatalogName,			SQLCHAR*)
		fXX(NameLength1,			SQLSMALLINT)
		fXX(SchemaName,				SQLCHAR*)
		fXX(NameLength2,			SQLSMALLINT)
		fXX(TableName,				SQLCHAR*)
		fXX(NameLength3,			SQLSMALLINT)
		fXX(TableType,				SQLCHAR*)
		fXX(NameLength4,			SQLSMALLINT)
	} args;
	CallFrameSQLTables(const Args &rArgs)
	: CallFrame(SQL_API_SQLTABLES, "SQLTables", SQL_HANDLE_STMT), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(StatementHandle,		SQLHSTMT)
			fHS(CatalogName,			SQLCHAR*)
			fHX(NameLength1,			SQLSMALLINT)
			fHS(SchemaName,				SQLCHAR*)
			fHX(NameLength2,			SQLSMALLINT)
			fHS(TableName,				SQLCHAR*)
			fHX(NameLength3,			SQLSMALLINT)
			fHS(TableType,				SQLCHAR*)
			fHX(NameLength4,			SQLSMALLINT)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLTables(CallFrameSQLTables::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLTables ? stack->Push(new CallFrameSQLTables(args)) : -1;
}

class CallFrameSQLTablesW : public CallFrame
{
public:
	struct Args
	{
		fXX(StatementHandle,		SQLHSTMT)
		fXX(CatalogName,			SQLWCHAR*)
		fXX(NameLength1,			SQLSMALLINT)
		fXX(SchemaName,				SQLWCHAR*)
		fXX(NameLength2,			SQLSMALLINT)
		fXX(TableName,				SQLWCHAR*)
		fXX(NameLength3,			SQLSMALLINT)
		fXX(TableType,				SQLWCHAR*)
		fXX(NameLength4,			SQLSMALLINT)
	} args;
	CallFrameSQLTablesW(const Args &rArgs)
	: CallFrame(SQL_API_SQLTABLES, "SQLTablesW", SQL_HANDLE_STMT), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(StatementHandle,		SQLHSTMT)
			fLS(CatalogName,			SQLWCHAR*)
			fHX(NameLength1,			SQLSMALLINT)
			fLS(SchemaName,				SQLWCHAR*)
			fHX(NameLength2,			SQLSMALLINT)
			fLS(TableName,				SQLWCHAR*)
			fHX(NameLength3,			SQLSMALLINT)
			fLS(TableType,				SQLWCHAR*)
			fHX(NameLength4,			SQLSMALLINT)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLTablesW(CallFrameSQLTablesW::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLTables ? stack->Push(new CallFrameSQLTablesW(args)) : -1;
}

class CallFrameSQLColumns : public CallFrame
{
public:
	struct Args
	{
		fXX(StatementHandle,		SQLHSTMT)
		fXX(CatalogName,			SQLCHAR*)
		fXX(NameLength1,			SQLSMALLINT)
		fXX(SchemaName,				SQLCHAR*)
		fXX(NameLength2,			SQLSMALLINT)
		fXX(TableName,				SQLCHAR*)
		fXX(NameLength3,			SQLSMALLINT)
		fXX(ColumnName,				SQLCHAR*)
		fXX(NameLength4,			SQLSMALLINT)
	} args;
	CallFrameSQLColumns(const Args &rArgs)
	: CallFrame(SQL_API_SQLCOLUMNS, "SQLColumns", SQL_HANDLE_STMT), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(StatementHandle,		SQLHSTMT)
			fHS(CatalogName,			SQLCHAR*)
			fHX(NameLength1,			SQLSMALLINT)
			fHS(SchemaName,				SQLCHAR*)
			fHX(NameLength2,			SQLSMALLINT)
			fHS(TableName,				SQLCHAR*)
			fHX(NameLength3,			SQLSMALLINT)
			fHS(ColumnName,				SQLCHAR*)
			fHX(NameLength4,			SQLSMALLINT)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLColumns(CallFrameSQLColumns::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLColumns ? stack->Push(new CallFrameSQLColumns(args)) : -1;
}

class CallFrameSQLColumnsW : public CallFrame
{
public:
	struct Args
	{
		fXX(StatementHandle,		SQLHSTMT)
		fXX(CatalogName,			SQLWCHAR*)
		fXX(NameLength1,			SQLSMALLINT)
		fXX(SchemaName,				SQLWCHAR*)
		fXX(NameLength2,			SQLSMALLINT)
		fXX(TableName,				SQLWCHAR*)
		fXX(NameLength3,			SQLSMALLINT)
		fXX(ColumnName,				SQLWCHAR*)
		fXX(NameLength4,			SQLSMALLINT)
	} args;
	CallFrameSQLColumnsW(const Args &rArgs)
	: CallFrame(SQL_API_SQLCOLUMNS, "SQLColumnsW", SQL_HANDLE_STMT), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(StatementHandle,		SQLHSTMT)
			fLS(CatalogName,			SQLWCHAR*)
			fHX(NameLength1,			SQLSMALLINT)
			fLS(SchemaName,				SQLWCHAR*)
			fHX(NameLength2,			SQLSMALLINT)
			fLS(TableName,				SQLWCHAR*)
			fHX(NameLength3,			SQLSMALLINT)
			fLS(ColumnName,				SQLWCHAR*)
			fHX(NameLength4,			SQLSMALLINT)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLColumnsW(CallFrameSQLColumnsW::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLColumns ? stack->Push(new CallFrameSQLColumnsW(args)) : -1;
}

class CallFrameSQLStatistics : public CallFrame
{
public:
	struct Args
	{
		fXX(StatementHandle,		SQLHSTMT)
		fXX(CatalogName,			SQLCHAR*)
		fXX(NameLength1,			SQLSMALLINT)
		fXX(SchemaName,				SQLCHAR*)
		fXX(NameLength2,			SQLSMALLINT)
		fXX(TableName,				SQLCHAR*)
		fXX(NameLength3,			SQLSMALLINT)
		fXX(Unique,					SQLUSMALLINT)
		fXX(Accuracy,				SQLUSMALLINT)
	} args;
	CallFrameSQLStatistics(const Args &rArgs)
	: CallFrame(SQL_API_SQLSTATISTICS, "SQLStatistics", SQL_HANDLE_STMT), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(StatementHandle,		SQLHSTMT)
			fHS(CatalogName,			SQLCHAR*)
			fHX(NameLength1,			SQLSMALLINT)
			fHS(SchemaName,				SQLCHAR*)
			fHX(NameLength2,			SQLSMALLINT)
			fHS(TableName,				SQLCHAR*)
			fHX(NameLength3,			SQLSMALLINT)
			fHX(Unique,					SQLUSMALLINT)
			fHX(Accuracy,				SQLUSMALLINT)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLStatistics(CallFrameSQLStatistics::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLStatistics ? stack->Push(new CallFrameSQLStatistics(args)) : -1;
}

class CallFrameSQLStatisticsW : public CallFrame
{
public:
	struct Args
	{
		fXX(StatementHandle,		SQLHSTMT)
		fXX(CatalogName,			SQLWCHAR*)
		fXX(NameLength1,			SQLSMALLINT)
		fXX(SchemaName,				SQLWCHAR*)
		fXX(NameLength2,			SQLSMALLINT)
		fXX(TableName,				SQLWCHAR*)
		fXX(NameLength3,			SQLSMALLINT)
		fXX(Unique,					SQLUSMALLINT)
		fXX(Accuracy,				SQLSMALLINT)
	} args;
	CallFrameSQLStatisticsW(const Args &rArgs)
	: CallFrame(SQL_API_SQLSTATISTICS, "SQLStatistics", SQL_HANDLE_STMT), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(StatementHandle,		SQLHSTMT)
			fLS(CatalogName,			SQLWCHAR*)
			fHX(NameLength1,			SQLSMALLINT)
			fLS(SchemaName,				SQLWCHAR*)
			fHX(NameLength2,			SQLSMALLINT)
			fLS(TableName,				SQLWCHAR*)
			fHX(NameLength3,			SQLSMALLINT)
			fHX(Unique,					SQLUSMALLINT)
			fHX(Accuracy,				SQLSMALLINT)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLStatisticsW(CallFrameSQLStatisticsW::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLStatistics ? stack->Push(new CallFrameSQLStatisticsW(args)) : -1;
}

class CallFrameSQLTablePrivileges : public CallFrame
{
public:
	struct Args
	{
		fXX(StatementHandle,		SQLHSTMT)
		fXX(CatalogName,			SQLCHAR*)
		fXX(NameLength1,			SQLSMALLINT)
		fXX(SchemaName,				SQLCHAR*)
		fXX(NameLength2,			SQLSMALLINT)
		fXX(TableName,				SQLCHAR*)
		fXX(NameLength3,			SQLSMALLINT)
	} args;
	CallFrameSQLTablePrivileges(const Args &rArgs)
	: CallFrame(SQL_API_SQLTABLEPRIVILEGES, "SQLTablePrivileges", SQL_HANDLE_STMT), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(StatementHandle,		SQLHSTMT)
			fHS(CatalogName,			SQLCHAR*)
			fHX(NameLength1,			SQLSMALLINT)
			fHS(SchemaName,				SQLCHAR*)
			fHX(NameLength2,			SQLSMALLINT)
			fHS(TableName,				SQLCHAR*)
			fHX(NameLength3,			SQLSMALLINT)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLTablePrivileges(CallFrameSQLTablePrivileges::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLTablePrivileges ? stack->Push(new CallFrameSQLTablePrivileges(args)) : -1;
}

class CallFrameSQLTablePrivilegesW : public CallFrame
{
public:
	struct Args
	{
		fXX(StatementHandle,		SQLHSTMT)
		fXX(CatalogName,			SQLWCHAR*)
		fXX(NameLength1,			SQLSMALLINT)
		fXX(SchemaName,				SQLWCHAR*)
		fXX(NameLength2,			SQLSMALLINT)
		fXX(TableName,				SQLWCHAR*)
		fXX(NameLength3,			SQLSMALLINT)
	} args;
	CallFrameSQLTablePrivilegesW(const Args &rArgs)
	: CallFrame(SQL_API_SQLTABLEPRIVILEGES, "SQLTablePrivilegesW", SQL_HANDLE_STMT), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(StatementHandle,		SQLHSTMT)
			fLS(CatalogName,			SQLWCHAR*)
			fHX(NameLength1,			SQLSMALLINT)
			fLS(SchemaName,				SQLWCHAR*)
			fHX(NameLength2,			SQLSMALLINT)
			fLS(TableName,				SQLWCHAR*)
			fHX(NameLength3,			SQLSMALLINT)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLTablePrivilegesW(CallFrameSQLTablePrivilegesW::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLTablePrivileges ? stack->Push(new CallFrameSQLTablePrivilegesW(args)) : -1;
}

class CallFrameSQLColumnPrivileges : public CallFrame
{
public:
	struct Args
	{
		fXX(StatementHandle,		SQLHSTMT)
		fXX(CatalogName,			SQLCHAR*)
		fXX(NameLength1,			SQLSMALLINT)
		fXX(SchemaName,				SQLCHAR*)
		fXX(NameLength2,			SQLSMALLINT)
		fXX(TableName,				SQLCHAR*)
		fXX(NameLength3,			SQLSMALLINT)
		fXX(ColumnName,				SQLCHAR*)
		fXX(NameLength4,			SQLSMALLINT)
	} args;
	CallFrameSQLColumnPrivileges(const Args &rArgs)
	: CallFrame(SQL_API_SQLCOLUMNPRIVILEGES, "SQLColumnPrivileges", SQL_HANDLE_STMT), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(StatementHandle,		SQLHSTMT)
			fHS(CatalogName,			SQLCHAR*)
			fHX(NameLength1,			SQLSMALLINT)
			fHS(SchemaName,				SQLCHAR*)
			fHX(NameLength2,			SQLSMALLINT)
			fHS(TableName,				SQLCHAR*)
			fHX(NameLength3,			SQLSMALLINT)
			fHS(ColumnName,				SQLCHAR*)
			fHX(NameLength4,			SQLSMALLINT)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLColumnPrivileges(CallFrameSQLColumnPrivileges::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLColumnPrivileges ? stack->Push(new CallFrameSQLColumnPrivileges(args)) : -1;
}

class CallFrameSQLColumnPrivilegesW : public CallFrame
{
public:
	struct Args
	{
		fXX(StatementHandle,		SQLHSTMT)
		fXX(CatalogName,			SQLWCHAR*)
		fXX(NameLength1,			SQLSMALLINT)
		fXX(SchemaName,				SQLWCHAR*)
		fXX(NameLength2,			SQLSMALLINT)
		fXX(TableName,				SQLWCHAR*)
		fXX(NameLength3,			SQLSMALLINT)
		fXX(ColumnName,				SQLWCHAR*)
		fXX(NameLength4,			SQLSMALLINT)
	} args;
	CallFrameSQLColumnPrivilegesW(const Args &rArgs)
	: CallFrame(SQL_API_SQLCOLUMNPRIVILEGES, "SQLColumnPrivilegesW", SQL_HANDLE_STMT), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(StatementHandle,		SQLHSTMT)
			fLS(CatalogName,			SQLWCHAR*)
			fHX(NameLength1,			SQLSMALLINT)
			fLS(SchemaName,				SQLWCHAR*)
			fHX(NameLength2,			SQLSMALLINT)
			fLS(TableName,				SQLWCHAR*)
			fHX(NameLength3,			SQLSMALLINT)
			fLS(ColumnName,				SQLWCHAR*)
			fHX(NameLength4,			SQLSMALLINT)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLColumnPrivilegesW(CallFrameSQLColumnPrivilegesW::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLColumnPrivileges ? stack->Push(new CallFrameSQLColumnPrivilegesW(args)) : -1;
}

class CallFrameSQLSpecialColumns : public CallFrame
{
public:
	struct Args
	{
		fXX(StatementHandle,		SQLHSTMT)
		fXX(IdentifierType,			SQLUSMALLINT)
		fXX(CatalogName,			SQLCHAR*)
		fXX(NameLength1,			SQLSMALLINT)
		fXX(SchemaName,				SQLCHAR*)
		fXX(NameLength2,			SQLSMALLINT)
		fXX(TableName,				SQLCHAR*)
		fXX(NameLength3,			SQLSMALLINT)
		fXX(Scope,					SQLUSMALLINT)
		fXX(Nullable,				SQLUSMALLINT)
	} args;
	CallFrameSQLSpecialColumns(const Args &rArgs)
	: CallFrame(SQL_API_SQLSPECIALCOLUMNS, "SQLSpecialColumns", SQL_HANDLE_STMT), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(StatementHandle,		SQLHSTMT)
			fHX(IdentifierType,			SQLUSMALLINT)
			fHS(CatalogName,			SQLCHAR*)
			fHX(NameLength1,			SQLSMALLINT)
			fHS(SchemaName,				SQLCHAR*)
			fHX(NameLength2,			SQLSMALLINT)
			fHS(TableName,				SQLCHAR*)
			fHX(NameLength3,			SQLSMALLINT)
			fHX(Scope,					SQLUSMALLINT)
			fHX(Nullable,				SQLUSMALLINT)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLSpecialColumns(CallFrameSQLSpecialColumns::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLSpecialColumns ? stack->Push(new CallFrameSQLSpecialColumns(args)) : -1;
}

class CallFrameSQLSpecialColumnsW : public CallFrame
{
public:
	struct Args
	{
		fXX(StatementHandle,		SQLHSTMT)
		fXX(IdentifierType,			SQLUSMALLINT)
		fXX(CatalogName,			SQLWCHAR*)
		fXX(NameLength1,			SQLSMALLINT)
		fXX(SchemaName,				SQLWCHAR*)
		fXX(NameLength2,			SQLSMALLINT)
		fXX(TableName,				SQLWCHAR*)
		fXX(NameLength3,			SQLSMALLINT)
		fXX(Scope,					SQLUSMALLINT)
		fXX(Nullable,				SQLUSMALLINT)
	} args;
	CallFrameSQLSpecialColumnsW(const Args &rArgs)
	: CallFrame(SQL_API_SQLSPECIALCOLUMNS, "SQLSpecialColumnsW", SQL_HANDLE_STMT), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(StatementHandle,		SQLHSTMT)
			fHX(IdentifierType,			SQLUSMALLINT)
			fLS(CatalogName,			SQLWCHAR*)
			fHX(NameLength1,			SQLSMALLINT)
			fLS(SchemaName,				SQLWCHAR*)
			fHX(NameLength2,			SQLSMALLINT)
			fLS(TableName,				SQLWCHAR*)
			fHX(NameLength3,			SQLSMALLINT)
			fHX(Scope,					SQLUSMALLINT)
			fHX(Nullable,				SQLUSMALLINT)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLSpecialColumnsW(CallFrameSQLSpecialColumnsW::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLSpecialColumns ? stack->Push(new CallFrameSQLSpecialColumnsW(args)) : -1;
}

class CallFrameSQLPrimaryKeys : public CallFrame
{
public:
	struct Args
	{
		fXX(StatementHandle,		SQLHSTMT)
		fXX(CatalogName,			SQLCHAR*)
		fXX(NameLength1,			SQLSMALLINT)
		fXX(SchemaName,				SQLCHAR*)
		fXX(NameLength2,			SQLSMALLINT)
		fXX(TableName,				SQLCHAR*)
		fXX(NameLength3,			SQLSMALLINT)
	} args;
	CallFrameSQLPrimaryKeys(const Args &rArgs)
	: CallFrame(SQL_API_SQLPRIMARYKEYS, "SQLPrimaryKeys", SQL_HANDLE_STMT), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(StatementHandle,		SQLHSTMT)
			fHS(CatalogName,			SQLCHAR*)
			fHX(NameLength1,			SQLSMALLINT)
			fHS(SchemaName,				SQLCHAR*)
			fHX(NameLength2,			SQLSMALLINT)
			fHS(TableName,				SQLCHAR*)
			fHX(NameLength3,			SQLSMALLINT)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLPrimaryKeys(CallFrameSQLPrimaryKeys::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLPrimaryKeys ? stack->Push(new CallFrameSQLPrimaryKeys(args)) : -1;
}

class CallFrameSQLPrimaryKeysW : public CallFrame
{
public:
	struct Args
	{
		fXX(StatementHandle,		SQLHSTMT)
		fXX(CatalogName,			SQLWCHAR*)
		fXX(NameLength1,			SQLSMALLINT)
		fXX(SchemaName,				SQLWCHAR*)
		fXX(NameLength2,			SQLSMALLINT)
		fXX(TableName,				SQLWCHAR*)
		fXX(NameLength3,			SQLSMALLINT)
	} args;
	CallFrameSQLPrimaryKeysW(const Args &rArgs)
	: CallFrame(SQL_API_SQLPRIMARYKEYS, "SQLPrimaryKeysW", SQL_HANDLE_STMT), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(StatementHandle,		SQLHSTMT)
			fLS(CatalogName,			SQLWCHAR*)
			fHX(NameLength1,			SQLSMALLINT)
			fLS(SchemaName,				SQLWCHAR*)
			fHX(NameLength2,			SQLSMALLINT)
			fLS(TableName,				SQLWCHAR*)
			fHX(NameLength3,			SQLSMALLINT)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLPrimaryKeysW(CallFrameSQLPrimaryKeysW::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLPrimaryKeys ? stack->Push(new CallFrameSQLPrimaryKeysW(args)) : -1;
}

class CallFrameSQLForeignKeys : public CallFrame
{
public:
	struct Args
	{
		fXX(StatementHandle,		SQLHSTMT)
		fXX(PKCatalogName,			SQLCHAR*)
		fXX(NameLength1,			SQLSMALLINT)
		fXX(PKSchemaName,			SQLCHAR*)
		fXX(NameLength2,			SQLSMALLINT)
		fXX(PKTableName,			SQLCHAR*)
		fXX(NameLength3,			SQLSMALLINT)
		fXX(FKCatalogName,			SQLCHAR*)
		fXX(NameLength4,			SQLSMALLINT)
		fXX(FKSchemaName,			SQLCHAR*)
		fXX(NameLength5,			SQLSMALLINT)
		fXX(FKTableName,			SQLCHAR*)
		fXX(NameLength6,			SQLSMALLINT)
	} args;
	CallFrameSQLForeignKeys(const Args &rArgs)
	: CallFrame(SQL_API_SQLFOREIGNKEYS, "SQLForeignKeys", SQL_HANDLE_STMT), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(StatementHandle,		SQLHSTMT)
			fHS(PKCatalogName,			SQLCHAR*)
			fHX(NameLength1,			SQLSMALLINT)
			fHS(PKSchemaName,			SQLCHAR*)
			fHX(NameLength2,			SQLSMALLINT)
			fHS(PKTableName,			SQLCHAR*)
			fHX(NameLength3,			SQLSMALLINT)
			fHS(FKCatalogName,			SQLCHAR*)
			fHX(NameLength4,			SQLSMALLINT)
			fHS(FKSchemaName,			SQLCHAR*)
			fHX(NameLength5,			SQLSMALLINT)
			fHS(FKTableName,			SQLCHAR*)
			fHX(NameLength6,			SQLSMALLINT)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLForeignKeys(CallFrameSQLForeignKeys::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLForeignKeys ? stack->Push(new CallFrameSQLForeignKeys(args)) : -1;
}

class CallFrameSQLForeignKeysW : public CallFrame
{
public:
	struct Args
	{
		fXX(StatementHandle,		SQLHSTMT)
		fXX(PKCatalogName,			SQLWCHAR*)
		fXX(NameLength1,			SQLSMALLINT)
		fXX(PKSchemaName,			SQLWCHAR*)
		fXX(NameLength2,			SQLSMALLINT)
		fXX(PKTableName,			SQLWCHAR*)
		fXX(NameLength3,			SQLSMALLINT)
		fXX(FKCatalogName,			SQLWCHAR*)
		fXX(NameLength4,			SQLSMALLINT)
		fXX(FKSchemaName,			SQLWCHAR*)
		fXX(NameLength5,			SQLSMALLINT)
		fXX(FKTableName,			SQLWCHAR*)
		fXX(NameLength6,			SQLSMALLINT)
	} args;
	CallFrameSQLForeignKeysW(const Args &rArgs)
	: CallFrame(SQL_API_SQLFOREIGNKEYS, "SQLForeignKeysW", SQL_HANDLE_STMT), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(StatementHandle,		SQLHSTMT)
			fLS(PKCatalogName,			SQLWCHAR*)
			fHX(NameLength1,			SQLSMALLINT)
			fLS(PKSchemaName,			SQLWCHAR*)
			fHX(NameLength2,			SQLSMALLINT)
			fLS(PKTableName,			SQLWCHAR*)
			fHX(NameLength3,			SQLSMALLINT)
			fLS(FKCatalogName,			SQLWCHAR*)
			fHX(NameLength4,			SQLSMALLINT)
			fLS(FKSchemaName,			SQLWCHAR*)
			fHX(NameLength5,			SQLSMALLINT)
			fLS(FKTableName,			SQLWCHAR*)
			fHX(NameLength6,			SQLSMALLINT)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLForeignKeysW(CallFrameSQLForeignKeysW::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLForeignKeys ? stack->Push(new CallFrameSQLForeignKeysW(args)) : -1;
}

class CallFrameSQLProcedures : public CallFrame
{
public:
	struct Args
	{
		fXX(StatementHandle,		SQLHSTMT)
		fXX(CatalogName,			SQLCHAR*)
		fXX(NameLength1,			SQLSMALLINT)
		fXX(SchemaName,				SQLCHAR*)
		fXX(NameLength2,			SQLSMALLINT)
		fXX(ProcName,				SQLCHAR*)
		fXX(NameLength3,			SQLSMALLINT)
	} args;
	CallFrameSQLProcedures(const Args &rArgs)
	: CallFrame(SQL_API_SQLPROCEDURES, "SQLProcedures", SQL_HANDLE_STMT), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(StatementHandle,		SQLHSTMT)
			fHS(CatalogName,			SQLCHAR*)
			fHX(NameLength1,			SQLSMALLINT)
			fHS(SchemaName,				SQLCHAR*)
			fHX(NameLength2,			SQLSMALLINT)
			fHS(ProcName,				SQLCHAR*)
			fHX(NameLength3,			SQLSMALLINT)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLProcedures(CallFrameSQLProcedures::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLProcedures ? stack->Push(new CallFrameSQLProcedures(args)) : -1;
}

class CallFrameSQLProceduresW : public CallFrame
{
public:
	struct Args
	{
		fXX(StatementHandle,		SQLHSTMT)
		fXX(CatalogName,			SQLWCHAR*)
		fXX(NameLength1,			SQLSMALLINT)
		fXX(SchemaName,				SQLWCHAR*)
		fXX(NameLength2,			SQLSMALLINT)
		fXX(ProcName,				SQLWCHAR*)
		fXX(NameLength3,			SQLSMALLINT)
	} args;
	CallFrameSQLProceduresW(const Args &rArgs)
	: CallFrame(SQL_API_SQLPROCEDURES, "SQLProceduresW", SQL_HANDLE_STMT), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(StatementHandle,		SQLHSTMT)
			fLS(CatalogName,			SQLWCHAR*)
			fHX(NameLength1,			SQLSMALLINT)
			fLS(SchemaName,				SQLWCHAR*)
			fHX(NameLength2,			SQLSMALLINT)
			fLS(ProcName,				SQLWCHAR*)
			fHX(NameLength3,			SQLSMALLINT)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLProceduresW(CallFrameSQLProceduresW::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLProcedures ? stack->Push(new CallFrameSQLProceduresW(args)) : -1;
}

class CallFrameSQLProcedureColumns : public CallFrame
{
public:
	struct Args
	{
		fXX(StatementHandle,		SQLHSTMT)
		fXX(CatalogName,			SQLCHAR*)
		fXX(NameLength1,			SQLSMALLINT)
		fXX(SchemaName,				SQLCHAR*)
		fXX(NameLength2,			SQLSMALLINT)
		fXX(ProcName,				SQLCHAR*)
		fXX(NameLength3,			SQLSMALLINT)
		fXX(ColumnName,				SQLCHAR*)
		fXX(NameLength4,			SQLSMALLINT)
	} args;
	CallFrameSQLProcedureColumns(const Args &rArgs)
	: CallFrame(SQL_API_SQLPROCEDURECOLUMNS, "SQLProcedureColumns", SQL_HANDLE_STMT), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(StatementHandle,		SQLHSTMT)
			fHS(CatalogName,			SQLCHAR*)
			fHX(NameLength1,			SQLSMALLINT)
			fHS(SchemaName,				SQLCHAR*)
			fHX(NameLength2,			SQLSMALLINT)
			fHS(ProcName,				SQLCHAR*)
			fHX(NameLength3,			SQLSMALLINT)
			fHS(ColumnName,				SQLCHAR*)
			fHX(NameLength4,			SQLSMALLINT)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLProcedureColumns(CallFrameSQLProcedureColumns::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLProcedureColumns ? stack->Push(new CallFrameSQLProcedureColumns(args)) : -1;
}

class CallFrameSQLProcedureColumnsW : public CallFrame
{
public:
	struct Args
	{
		fXX(StatementHandle,		SQLHSTMT)
		fXX(CatalogName,			SQLWCHAR*)
		fXX(NameLength1,			SQLSMALLINT)
		fXX(SchemaName,				SQLWCHAR*)
		fXX(NameLength2,			SQLSMALLINT)
		fXX(ProcName,				SQLWCHAR*)
		fXX(NameLength3,			SQLSMALLINT)
		fXX(ColumnName,				SQLWCHAR*)
		fXX(NameLength4,			SQLSMALLINT)
	} args;
	CallFrameSQLProcedureColumnsW(const Args &rArgs)
	: CallFrame(SQL_API_SQLPROCEDURECOLUMNS, "SQLProcedureColumnsW", SQL_HANDLE_STMT), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(StatementHandle,		SQLHSTMT)
			fLS(CatalogName,			SQLWCHAR*)
			fHX(NameLength1,			SQLSMALLINT)
			fLS(SchemaName,				SQLWCHAR*)
			fHX(NameLength2,			SQLSMALLINT)
			fLS(ProcName,				SQLWCHAR*)
			fHX(NameLength3,			SQLSMALLINT)
			fLS(ColumnName,				SQLWCHAR*)
			fHX(NameLength4,			SQLSMALLINT)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLProcedureColumnsW(CallFrameSQLProcedureColumnsW::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLProcedureColumns ? stack->Push(new CallFrameSQLProcedureColumnsW(args)) : -1;
}

class CallFrameSQLConnect : public CallFrame
{
public:
	struct Args
	{
		fXX(hdbc,					SQLHDBC)
		fXX(ServerName,				SQLCHAR*)
		fXX(NameLength1,			SQLSMALLINT)
		fXX(UserName,				SQLCHAR*)
		fXX(NameLength2,			SQLSMALLINT)
		fXX(Authentication,			SQLCHAR*)
		fXX(NameLength3,			SQLSMALLINT)
	} args;
	CallFrameSQLConnect(const Args &rArgs)
	: CallFrame(SQL_API_SQLCONNECT, "SQLConnect", SQL_HANDLE_DBC), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(hdbc,					SQLHDBC)
			fHS(ServerName,				SQLCHAR*)
			fHX(NameLength1,			SQLSMALLINT)
			fHS(UserName,				SQLCHAR*)
			fHX(NameLength2,			SQLSMALLINT)
			fHS(Authentication,			SQLCHAR*)
			fHX(NameLength3,			SQLSMALLINT)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLConnect(CallFrameSQLConnect::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLConnect ? stack->Push(new CallFrameSQLConnect(args)) : -1;
}

class CallFrameSQLConnectW : public CallFrame
{
public:
	struct Args
	{
		fXX(hdbc,					SQLHDBC)
		fXX(ServerName,				SQLWCHAR*)
		fXX(NameLength1,			SQLSMALLINT)
		fXX(UserName,				SQLWCHAR*)
		fXX(NameLength2,			SQLSMALLINT)
		fXX(Authentication,			SQLWCHAR*)
		fXX(NameLength3,			SQLSMALLINT)
	} args;
	CallFrameSQLConnectW(const Args &rArgs)
	: CallFrame(SQL_API_SQLCONNECT, "SQLConnectW", SQL_HANDLE_DBC), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(hdbc,					SQLHDBC)
			fLS(ServerName,				SQLWCHAR*)
			fHX(NameLength1,			SQLSMALLINT)
			fLS(UserName,				SQLWCHAR*)
			fHX(NameLength2,			SQLSMALLINT)
			fLS(Authentication,			SQLWCHAR*)
			fHX(NameLength3,			SQLSMALLINT)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLConnectW(CallFrameSQLConnectW::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLConnect ? stack->Push(new CallFrameSQLConnectW(args)) : -1;
}

class CallFrameSQLDriverConnect : public CallFrame
{
public:
	struct Args
	{
		fXX(hdbc,					SQLHDBC)
		fXX(hwnd,					SQLHWND)
		fXX(InConnectionString,		SQLCHAR*)
		fXX(NameLength1,			SQLSMALLINT)
		fXX(OutConnectionString,	SQLCHAR*)
		fXX(BufferLength,			SQLSMALLINT)
		fXX(StringLength2Ptr,		SQLSMALLINT*)
		fXX(DriverCompletion,		SQLUSMALLINT)
	} args;
	Legend DriverCompletion;
	CallFrameSQLDriverConnect(const Args &rArgs)
	: CallFrame(SQL_API_SQLDRIVERCONNECT, "SQLDriverConnect", SQL_HANDLE_DBC), args(rArgs)
	, DriverCompletion(args.DriverCompletion, Legend::SqlDriverCompletion)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(hdbc,					SQLHDBC)
			fGX(hwnd,					SQLHWND)
			fHS(InConnectionString,		SQLCHAR*)
			fHX(NameLength1,			SQLSMALLINT)
			fPX(OutConnectionString,	SQLCHAR*)
			fHX(BufferLength,			SQLSMALLINT)
			fPX(StringLength2Ptr,		SQLSMALLINT*)
			fHA(DriverCompletion,		SQLUSMALLINT)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLDriverConnect(CallFrameSQLDriverConnect::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLDriverConnect ? stack->Push(new CallFrameSQLDriverConnect(args)) : -1;
}

class CallFrameSQLDriverConnectW : public CallFrame
{
public:
	struct Args
	{
		fXX(hdbc,					SQLHDBC)
		fXX(hwnd,					SQLHWND)
		fXX(InConnectionString,		SQLWCHAR*)
		fXX(NameLength1,			SQLSMALLINT)
		fXX(OutConnectionString,	SQLWCHAR*)
		fXX(BufferLength,			SQLSMALLINT)
		fXX(StringLength2Ptr,		SQLSMALLINT*)
		fXX(DriverCompletion,		SQLUSMALLINT)
	} args;
	Legend DriverCompletion;
	CallFrameSQLDriverConnectW(const Args &rArgs)
	: CallFrame(SQL_API_SQLDRIVERCONNECT, "SQLDriverConnectW", SQL_HANDLE_DBC), args(rArgs)
	, DriverCompletion(args.DriverCompletion, Legend::SqlDriverCompletion)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(hdbc,					SQLHDBC)
			fGX(hwnd,					SQLHWND)
			fLS(InConnectionString,		SQLWCHAR*)
			fHX(NameLength1,			SQLSMALLINT)
			fPX(OutConnectionString,	SQLWCHAR*)
			fHX(BufferLength,			SQLSMALLINT)
			fPX(StringLength2Ptr,		SQLSMALLINT*)
			fHA(DriverCompletion,		SQLUSMALLINT)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLDriverConnectW(CallFrameSQLDriverConnectW::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLDriverConnect ? stack->Push(new CallFrameSQLDriverConnectW(args)) : -1;
}

class CallFrameSQLBrowseConnect : public CallFrame
{
public:
	struct Args
	{
		fXX(hdbc,					SQLHDBC)
		fXX(InConnectionString,		SQLCHAR*)
		fXX(NameLength1,			SQLSMALLINT)
		fXX(OutConnectionString,	SQLCHAR*)
		fXX(BufferLength,			SQLSMALLINT)
		fXX(StringLength2Ptr,		SQLSMALLINT*)
	} args;
	CallFrameSQLBrowseConnect(const Args &rArgs)
	: CallFrame(SQL_API_SQLBROWSECONNECT, "SQLBrowseConnect", SQL_HANDLE_DBC), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(hdbc,					SQLHDBC)
			fHS(InConnectionString,		SQLCHAR*)
			fHX(NameLength1,			SQLSMALLINT)
			fPX(OutConnectionString,	SQLCHAR*)
			fHX(BufferLength,			SQLSMALLINT)
			fPX(StringLength2Ptr,		SQLSMALLINT*)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLBrowseConnect(CallFrameSQLBrowseConnect::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLBrowseConnect ? stack->Push(new CallFrameSQLBrowseConnect(args)) : -1;
}

class CallFrameSQLBrowseConnectW : public CallFrame
{
public:
	struct Args
	{
		fXX(hdbc,					SQLHDBC)
		fXX(InConnectionString,		SQLWCHAR*)
		fXX(NameLength1,			SQLSMALLINT)
		fXX(OutConnectionString,	SQLWCHAR*)
		fXX(BufferLength,			SQLSMALLINT)
		fXX(StringLength2Ptr,		SQLSMALLINT*)
	} args;
	CallFrameSQLBrowseConnectW(const Args &rArgs)
	: CallFrame(SQL_API_SQLBROWSECONNECT, "SQLBrowseConnectW", SQL_HANDLE_DBC), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(hdbc,					SQLHDBC)
			fLS(InConnectionString,		SQLWCHAR*)
			fHX(NameLength1,			SQLSMALLINT)
			fPX(OutConnectionString,	SQLWCHAR*)
			fHX(BufferLength,			SQLSMALLINT)
			fPX(StringLength2Ptr,		SQLSMALLINT*)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLBrowseConnectW(CallFrameSQLBrowseConnectW::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLBrowseConnect ? stack->Push(new CallFrameSQLBrowseConnectW(args)) : -1;
}

class CallFrameSQLDisconnect : public CallFrame
{
public:
	struct Args
	{
		fXX(hdbc,					SQLHDBC)
	} args;
	CallFrameSQLDisconnect(const Args &rArgs)
	: CallFrame(SQL_API_SQLDISCONNECT, "SQLDisconnect", SQL_HANDLE_DBC), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(hdbc,					SQLHDBC)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLDisconnect(CallFrameSQLDisconnect::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLDisconnect ? stack->Push(new CallFrameSQLDisconnect(args)) : -1;
}

class CallFrameSQLEndTran : public CallFrame
{
public:
	struct Args
	{
		fXX(HandleType,				SQLSMALLINT)
		fXX(Handle,					SQLHANDLE)
		fXX(CompletionType,			SQLSMALLINT)
	} args;
	Legend CompletionType;
	CallFrameSQLEndTran(const Args &rArgs)
	: CallFrame(SQL_API_SQLENDTRAN, "SQLEndTran", rArgs.HandleType, FIELD_OFFSET(Args, Handle))
	, args(rArgs)
	, CompletionType(args.CompletionType, Legend::SqlCompletionType)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(HandleType,				SQLSMALLINT)
			fGX(Handle,					SQLHANDLE)
			fHA(CompletionType,			SQLSMALLINT)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLEndTran(CallFrameSQLEndTran::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLEndTran ? stack->Push(new CallFrameSQLEndTran(args)) : -1;
}

class CallFrameSQLTransact : public CallFrame
{
public:
	struct Args
	{
		fXX(EnvironmentHandle,		SQLHENV)
		fXX(ConnectionHandle,		SQLHDBC)
		fXX(CompletionType,			SQLUSMALLINT)
	} args;
	Legend CompletionType;
	CallFrameSQLTransact(const Args &rArgs)
	: CallFrame(SQL_API_SQLTRANSACT, "SQLTransact", SQL_HANDLE_ENV), args(rArgs)
	, CompletionType(args.CompletionType, Legend::SqlCompletionType)
	{
		if (args.ConnectionHandle)
		{
			CallFrame::HandleType = SQL_HANDLE_DBC;
			CallFrame::HandleOffset = FIELD_OFFSET(Args, ConnectionHandle);
		}
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(EnvironmentHandle,		SQLHENV)
			fGX(ConnectionHandle,		SQLHDBC)
			fHA(CompletionType,			SQLUSMALLINT)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLTransact(CallFrameSQLTransact::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLTransact ? stack->Push(new CallFrameSQLTransact(args)) : -1;
}

class CallFrameSQLNumResultCols : public CallFrame
{
public:
	struct Args
	{
		fXX(StatementHandle,		SQLHSTMT)
		fXX(ColumnCountPtr,			SQLSMALLINT*)
	} args;
	CallFrameSQLNumResultCols(const Args &rArgs)
	: CallFrame(SQL_API_SQLNUMRESULTCOLS, "SQLNumResultCols", SQL_HANDLE_STMT), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(StatementHandle,		SQLHSTMT)
			fPX(ColumnCountPtr,			SQLSMALLINT*)
		);
		CallFrame::Dump(fmt);
	}
	virtual void DumpResults()
	{
		static const char fmt[]
		(
			fHA(*ColumnCountPtr,		SQLSMALLINT)
		);
		CallFrame::DumpResults(fmt, Legend(args.ColumnCountPtr).text);
	}
};

RETCODE SQL_API TraceSQLNumResultCols(CallFrameSQLNumResultCols::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLNumResultCols ? stack->Push(new CallFrameSQLNumResultCols(args)) : -1;
}

class CallFrameSQLDescribeCol : public CallFrame
{
public:
	struct Args
	{
		fXX(StatementHandle,	SQLHSTMT)
		fXX(ColumnNumber,		SQLUSMALLINT)
		fXX(ColumnName,			SQLCHAR*)
		fXX(BufferLength,		SQLSMALLINT)
		fXX(NameLengthPtr,		SQLSMALLINT*)
		fXX(DataTypePtr,		SQLSMALLINT*)
		fXX(ColumnSizePtr,		SQLUINTEGER*)
		fXX(DecimalDigitsPtr,	SQLSMALLINT*)
		fXX(NullablePtr,		SQLSMALLINT*)
	} args;
	CallFrameSQLDescribeCol(const Args &rArgs)
	: CallFrame(SQL_API_SQLDESCRIBECOL, "SQLDescribeCol"), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(StatementHandle,		SQLHSTMT)
			fHX(ColumnNumber,			SQLUSMALLINT)
			fPX(ColumnName,				SQLCHAR*)
			fHX(BufferLength,			SQLSMALLINT)
			fPX(NameLengthPtr,			SQLSMALLINT*)
			fPX(DataTypePtr,			SQLSMALLINT*)
			fPX(ColumnSizePtr,			SQLUINTEGER*)
			fPX(DecimalDigitsPtr,		SQLSMALLINT*)
			fPX(NullablePtr,			SQLSMALLINT*)
		);
		CallFrame::Dump(fmt);
	}
	virtual void DumpResults()
	{
		static const char fmt[]
		(
			fHS(ColumnName,				SQLCHAR*)
			fHA(*DataTypePtr,			SQLSMALLINT)
			fHA(*ColumnSizePtr,			SQLUINTEGER)
			fHA(*DecimalDigitsPtr,		SQLSMALLINT)
			fHA(*NullablePtr,			SQLSMALLINT)
		);
		CallFrame::DumpResults
		(
			fmt
			,args.ColumnName
			,Legend(args.DataTypePtr, Legend::SqlType).text
			,Legend(args.ColumnSizePtr).text
			,Legend(args.DecimalDigitsPtr).text
			,Legend(args.NullablePtr).text
		);
	}
};

RETCODE SQL_API TraceSQLDescribeCol(CallFrameSQLDescribeCol::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLDescribeCol ? stack->Push(new CallFrameSQLDescribeCol(args)) : -1;
}

class CallFrameSQLDescribeColW : public CallFrame
{
public:
	struct Args
	{
		fXX(StatementHandle,		SQLHSTMT)
		fXX(ColumnNumber,			SQLUSMALLINT)
		fXX(ColumnName,				SQLWCHAR*)
		fXX(BufferLength,			SQLSMALLINT)
		fXX(NameLengthPtr,			SQLSMALLINT*)
		fXX(DataTypePtr,			SQLSMALLINT*)
		fXX(ColumnSizePtr,			SQLUINTEGER*)
		fXX(DecimalDigitsPtr,		SQLSMALLINT*)
		fXX(NullablePtr,			SQLSMALLINT*)
	} args;
	CallFrameSQLDescribeColW(const Args &rArgs)
	: CallFrame(SQL_API_SQLDESCRIBECOL, "SQLDescribeColW"), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(StatementHandle,		SQLHSTMT)
			fHX(ColumnNumber,			SQLUSMALLINT)
			fPX(ColumnName,				SQLWCHAR*)
			fHX(BufferLength,			SQLSMALLINT)
			fPX(NameLengthPtr,			SQLSMALLINT*)
			fPX(DataTypePtr,			SQLSMALLINT*)
			fPX(ColumnSizePtr,			SQLUINTEGER*)
			fPX(DecimalDigitsPtr,		SQLSMALLINT*)
			fPX(NullablePtr,			SQLSMALLINT*)
		);
		CallFrame::Dump(fmt);
	}
	virtual void DumpResults()
	{
		static const char fmt[]
		(
			fLS(ColumnName,				SQLWCHAR*)
			fHA(*DataTypePtr,			SQLSMALLINT)
			fHA(*ColumnSizePtr,			SQLUINTEGER)
			fHA(*DecimalDigitsPtr,		SQLSMALLINT)
			fHA(*NullablePtr,			SQLSMALLINT)
		);
		CallFrame::DumpResults
		(
			fmt
			,args.ColumnName
			,Legend(args.DataTypePtr, Legend::SqlType).text
			,Legend(args.ColumnSizePtr).text
			,Legend(args.DecimalDigitsPtr).text
			,Legend(args.NullablePtr).text
		);
	}
};

RETCODE SQL_API TraceSQLDescribeColW(CallFrameSQLDescribeColW::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLDescribeCol ? stack->Push(new CallFrameSQLDescribeColW(args)) : -1;
}

class CallFrameSQLColAttribute : public CallFrame
{
public:
	struct Args
	{
		fXX(StatementHandle,		SQLHSTMT)
		fXX(ColumnNumber,			SQLUSMALLINT)
		fXX(FieldIdentifier,		SQLUSMALLINT)
		fXX(CharacterAttributePtr,	SQLPOINTER)
		fXX(BufferLength,			SQLSMALLINT)
		fXX(StringLengthPtr,		SQLSMALLINT*)
		fXX(NumericAttributePtr,	SQLPOINTER)
	} args;
	CallFrameSQLColAttribute(const Args &rArgs)
	: CallFrame(SQL_API_SQLCOLATTRIBUTE, "SQLColAttribute"), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(StatementHandle,		SQLHSTMT)
			fHX(ColumnNumber,			SQLUSMALLINT)
			fHX(FieldIdentifier,		SQLUSMALLINT)
			fPX(CharacterAttributePtr,	SQLPOINTER)
			fHX(BufferLength,			SQLSMALLINT)
			fPX(StringLengthPtr,		SQLSMALLINT*)
			fPX(NumericAttributePtr,	SQLPOINTER)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLColAttribute(CallFrameSQLColAttribute::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLColAttribute ? stack->Push(new CallFrameSQLColAttribute(args)) : -1;
}

class CallFrameSQLColAttributeW : public CallFrame
{
public:
	struct Args
	{
		fXX(StatementHandle,		SQLHSTMT)
		fXX(ColumnNumber,			SQLUSMALLINT)
		fXX(FieldIdentifier,		SQLUSMALLINT)
		fXX(CharacterAttributePtr,	SQLPOINTER)
		fXX(BufferLength,			SQLSMALLINT)
		fXX(StringLengthPtr,		SQLSMALLINT*)
		fXX(NumericAttributePtr,	SQLPOINTER)
	} args;
	CallFrameSQLColAttributeW(const Args &rArgs)
	: CallFrame(SQL_API_SQLCOLATTRIBUTE, "SQLColAttributeW"), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(StatementHandle,		SQLHSTMT)
			fHX(ColumnNumber,			SQLUSMALLINT)
			fHX(FieldIdentifier,		SQLUSMALLINT)
			fPX(CharacterAttributePtr,	SQLPOINTER)
			fHX(BufferLength,			SQLSMALLINT)
			fPX(StringLengthPtr,		SQLSMALLINT*)
			fPX(NumericAttributePtr,	SQLPOINTER)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLColAttributeW(CallFrameSQLColAttributeW::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLColAttribute ? stack->Push(new CallFrameSQLColAttributeW(args)) : -1;
}

class CallFrameSQLColAttributes : public CallFrame
{
public:
	struct Args
	{
		fXX(hstmt,					SQLHSTMT)
		fXX(icol,					SQLUSMALLINT)
		fXX(fDescType,				SQLUSMALLINT)
		fXX(rgbDesc,				SQLPOINTER)
		fXX(cbDescMax,				SQLSMALLINT)
		fXX(pcbDesc,				SQLSMALLINT*)
		fXX(pfDesc,					SQLINTEGER*)
	} args;
	CallFrameSQLColAttributes(const Args &rArgs)
	: CallFrame(SQL_API_SQLCOLATTRIBUTES, "SQLColAttributes"), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(hstmt,					SQLHSTMT)
			fHX(icol,					SQLUSMALLINT)
			fHX(fDescType,				SQLUSMALLINT)
			fPX(rgbDesc,				SQLPOINTER)
			fHX(cbDescMax,				SQLSMALLINT)
			fPX(pcbDesc,				SQLSMALLINT*)
			fPX(pfDesc,					SQLINTEGER*)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLColAttributes(CallFrameSQLColAttributes::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLColAttributes ? stack->Push(new CallFrameSQLColAttributes(args)) : -1;
}

class CallFrameSQLColAttributesW : public CallFrame
{
public:
	struct Args
	{
		fXX(hstmt,					SQLHSTMT)
		fXX(icol,					SQLUSMALLINT)
		fXX(fDescType,				SQLUSMALLINT)
		fXX(rgbDesc,				SQLPOINTER)
		fXX(cbDescMax,				SQLSMALLINT)
		fXX(pcbDesc,				SQLSMALLINT*)
		fXX(pfDesc,					SQLINTEGER*)
	} args;
	CallFrameSQLColAttributesW(const Args &rArgs)
	: CallFrame(SQL_API_SQLCOLATTRIBUTES, "SQLColAttributesW"), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(hstmt,					SQLHSTMT)
			fHX(icol,					SQLUSMALLINT)
			fHX(fDescType,				SQLUSMALLINT)
			fPX(rgbDesc,				SQLPOINTER)
			fHX(cbDescMax,				SQLSMALLINT)
			fPX(pcbDesc,				SQLSMALLINT*)
			fPX(pfDesc,					SQLINTEGER*)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLColAttributesW(CallFrameSQLColAttributesW::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLColAttributes ? stack->Push(new CallFrameSQLColAttributesW(args)) : -1;
}

class CallFrameSQLBindCol : public CallFrame
{
public:
	struct Args
	{
		fXX(StatementHandle,		SQLHSTMT)
		fXX(ColumnNumber,			SQLUSMALLINT)
		fXX(TargetType,				SQLSMALLINT)
		fXX(TargetValuePtr,			SQLPOINTER)
		fXX(BufferLength,			SQLINTEGER)
		fXX(StrLen_or_Ind,			SQLINTEGER*)
	} args;
	CallFrameSQLBindCol(const Args &rArgs)
	: CallFrame(SQL_API_SQLBINDCOL, "SQLBindCol"), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(StatementHandle,		SQLHSTMT)
			fHX(ColumnNumber,			SQLUSMALLINT)
			fHX(TargetType,				SQLSMALLINT)
			fPX(TargetValuePtr,			SQLPOINTER)
			fGX(BufferLength,			SQLINTEGER)
			fPX(StrLen_or_Ind,			SQLINTEGER*)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLBindCol(CallFrameSQLBindCol::Args args)
{
	if (stack->depth < RETCODE(ODBCTrace.SQLFetch | ODBCTrace.SQLFetchScroll | ODBCTrace.SQLExtendedFetch | ODBCTrace.SQLSetPos))
	{
		::EnterCriticalSection(&CallStack::cs);
		if (StatementInfo *pInfo = StatementInfo::FromHandle(args.StatementHandle, 0, args.ColumnNumber))
		{
			if (StatementInfo::Param *pParam = pInfo->GetColumn(args.ColumnNumber))
			{
				pParam->ValueType			= args.TargetType;
				pParam->ParameterValuePtr	= args.TargetValuePtr;
				pParam->BufferLength		= args.BufferLength;
				pParam->StrLen_or_IndPtr	= args.StrLen_or_Ind;
			}
		}
		::LeaveCriticalSection(&CallStack::cs);
	}
	return stack->HideLevel() < ODBCTrace.SQLBindCol ? stack->Push(new CallFrameSQLBindCol(args)) : -1;
}

class CallFrameSQLFetch : public CallFrame
{
public:
	struct Args
	{
		fXX(StatementHandle,		SQLHSTMT)
	} args;
	CallFrameSQLFetch(const Args &rArgs)
	: CallFrame(SQL_API_SQLFETCH, "SQLFetch"), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(StatementHandle,		SQLHSTMT)
		);
		CallFrame::Dump(fmt);
	}
	virtual char *DumpDiagFields(RETCODE result)
	{
		if (depth || result == SQL_NO_DATA)
			return 0;
		return StatementInfo::ColumnDump(args.StatementHandle);
	}
};

RETCODE SQL_API TraceSQLFetch(CallFrameSQLFetch::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLFetch ? stack->Push(new CallFrameSQLFetch(args)) : -1;
}

class CallFrameSQLGetData : public CallFrame
{
public:
	struct Args
	{
		fXX(StatementHandle,		SQLHSTMT)
		fXX(ColumnNumber,			SQLUSMALLINT)
		fXX(TargetType,				SQLSMALLINT)
		fXX(TargetValuePtr,			SQLPOINTER)
		fXX(BufferLength,			SQLINTEGER)
		fXX(StrLen_or_IndPtr,		SQLINTEGER*)
	} args;
	CallFrameSQLGetData(const Args &rArgs)
	: CallFrame(SQL_API_SQLGETDATA, "SQLGetData"), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(StatementHandle,		SQLHSTMT)
			fHX(ColumnNumber,			SQLUSMALLINT)
			fHX(TargetType,				SQLSMALLINT)
			fPX(TargetValuePtr,			SQLPOINTER)
			fGX(BufferLength,			SQLINTEGER)
			fPX(StrLen_or_IndPtr,		SQLINTEGER*)
		);
		CallFrame::Dump(fmt);
	}
	virtual char *DumpDiagFields(RETCODE result)
	{
		if (depth || result == SQL_NO_DATA)
			return 0;
		return StatementInfo::UnboundColumnDump(args.StatementHandle,
			args.ColumnNumber, args.TargetType, args.TargetValuePtr,
			args.BufferLength, args.StrLen_or_IndPtr);
	}
};

RETCODE SQL_API TraceSQLGetData(CallFrameSQLGetData::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLGetData ? stack->Push(new CallFrameSQLGetData(args)) : -1;
}

class CallFrameSQLMoreResults : public CallFrame
{
public:
	struct Args
	{
		fXX(StatementHandle,		SQLHSTMT)
	} args;
	CallFrameSQLMoreResults(const Args &rArgs)
	: CallFrame(SQL_API_SQLMORERESULTS, "SQLMoreResults"), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(StatementHandle,		SQLHSTMT)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLMoreResults(CallFrameSQLMoreResults::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLMoreResults ? stack->Push(new CallFrameSQLMoreResults(args)) : -1;
}

class CallFrameSQLRowCount : public CallFrame
{
public:
	struct Args
	{
		fXX(StatementHandle,		SQLHSTMT)
		fXX(RowCountPtr,			SQLINTEGER*)
	} args;
	CallFrameSQLRowCount(const Args &rArgs)
	: CallFrame(SQL_API_SQLROWCOUNT, "SQLRowCount"), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(StatementHandle,		SQLHSTMT)
			fPX(RowCountPtr,			SQLINTEGER*)
		);
		CallFrame::Dump(fmt);
	}
	virtual void DumpResults()
	{
		static const char fmt[]
		(
			fHA(*RowCountPtr,			SQLINTEGER)
		);
		CallFrame::DumpResults(fmt, Legend(args.RowCountPtr).text);
	}
};

RETCODE SQL_API TraceSQLRowCount(CallFrameSQLRowCount::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLRowCount ? stack->Push(new CallFrameSQLRowCount(args)) : -1;
}

class CallFrameSQLExtendedFetch : public CallFrame
{
public:
	struct Args
	{
		fXX(StatementHandle,		SQLHSTMT)
		fXX(FetchOrientation,		SQLUSMALLINT)
		fXX(FetchOffset,			SQLINTEGER)
		fXX(RowCountPtr,			SQLUINTEGER*)
		fXX(RowStatusArray,			SQLUSMALLINT*)
	} args;
	CallFrameSQLExtendedFetch(const Args &rArgs)
	: CallFrame(SQL_API_SQLEXTENDEDFETCH, "SQLExtendedFetch"), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(StatementHandle,		SQLHSTMT)
			fHX(FetchOrientation,		SQLUSMALLINT)
			fGX(FetchOffset,			SQLINTEGER)
			fPX(RowCountPtr,			SQLUINTEGER*)
			fPX(RowStatusArray,			SQLUSMALLINT*)
		);
		CallFrame::Dump(fmt);
	}
	virtual void DumpResults()
	{
		static const char fmt[]
		(
			fHA(*RowCountPtr,			SQLUINTEGER)
		);
		CallFrame::DumpResults(fmt, Legend(args.RowCountPtr).text);
	}
	virtual char *DumpDiagFields(RETCODE result)
	{
		if (/*depth || */result == SQL_NO_DATA)
			return 0;
		return StatementInfo::ColumnDump(args.StatementHandle, *args.RowCountPtr);
	}
};

RETCODE SQL_API TraceSQLExtendedFetch(CallFrameSQLExtendedFetch::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLExtendedFetch ? stack->Push(new CallFrameSQLExtendedFetch(args)) : -1;
}

class CallFrameSQLFetchScroll : public CallFrame
{
public:
	struct Args
	{
		fXX(StatementHandle,		SQLHSTMT)
		fXX(FetchOrientation,		SQLUSMALLINT)
		fXX(FetchOffset,			SQLINTEGER)
	} args;
	CallFrameSQLFetchScroll(const Args &rArgs)
	: CallFrame(SQL_API_SQLFETCHSCROLL, "SQLFetchScroll"), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(StatementHandle,		SQLHSTMT)
			fHX(FetchOrientation,		SQLUSMALLINT)
			fGX(FetchOffset,			SQLINTEGER)
		);
		CallFrame::Dump(fmt);
	}
	virtual char *DumpDiagFields(RETCODE result)
	{
		if (depth || result == SQL_NO_DATA)
			return 0;
		return StatementInfo::ColumnDump(args.StatementHandle);
	}
};

RETCODE SQL_API TraceSQLFetchScroll(CallFrameSQLFetchScroll::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLFetchScroll ? stack->Push(new CallFrameSQLFetchScroll(args)) : -1;
}

class CallFrameSQLSetConnectOption : public CallFrame
{
public:
	struct Args
	{
		fXX(ConnectionHandle,		SQLHDBC)
		fXX(Option,					SQLUSMALLINT)
		fXX(Value,					SQLUINTEGER)
	} args;
	Legend Option, Value;
	CallFrameSQLSetConnectOption(const Args &rArgs)
	: CallFrame(SQL_API_SQLSETCONNECTOPTION, "SQLSetConnectOption", SQL_HANDLE_DBC), args(rArgs)
	, Option(args.Option, Legend::SqlConnectOption)
	, Value(args.Value,
		rArgs.Option == SQL_ODBC_CURSORS ? Legend::SqlAttrOdbcCursors :
		0)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(ConnectionHandle,		SQLHDBC)
			fHA(Option,					SQLUSMALLINT)
			fHA(Value,					SQLUINTEGER)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLSetConnectOption(CallFrameSQLSetConnectOption::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLSetConnectOption ? stack->Push(new CallFrameSQLSetConnectOption(args)) : -1;
}

class CallFrameSQLSetConnectOptionW : public CallFrame
{
public:
	struct Args
	{
		fXX(ConnectionHandle,		SQLHDBC)
		fXX(Option,					SQLUSMALLINT)
		fXX(Value,					SQLUINTEGER)
	} args;
	Legend Option;
	CallFrameSQLSetConnectOptionW(const Args &rArgs)
	: CallFrame(SQL_API_SQLSETCONNECTOPTION, "SQLSetConnectOptionW"), args(rArgs)
	, Option(args.Option, Legend::SqlConnectOption)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(ConnectionHandle,		SQLHDBC)
			fHA(Option,					SQLUSMALLINT)
			fGX(Value,					SQLUINTEGER)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLSetConnectOptionW(CallFrameSQLSetConnectOptionW::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLSetConnectOption ? stack->Push(new CallFrameSQLSetConnectOptionW(args)) : -1;
}

class CallFrameSQLGetConnectOption : public CallFrame
{
public:
	struct Args
	{
		fXX(ConnectionHandle,		SQLHDBC)
		fXX(Option,					SQLUSMALLINT)
		fXX(Value,					SQLPOINTER)
	} args;
	Legend Option;
	CallFrameSQLGetConnectOption(const Args &rArgs)
	: CallFrame(SQL_API_SQLGETCONNECTOPTION, "SQLGetConnectOption"), args(rArgs)
	, Option(args.Option, Legend::SqlConnectOption)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(ConnectionHandle,		SQLHDBC)
			fHA(Option,					SQLUSMALLINT)
			fPX(Value,					SQLPOINTER)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLGetConnectOption(CallFrameSQLGetConnectOption::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLGetConnectOption ? stack->Push(new CallFrameSQLGetConnectOption(args)) : -1;
}

class CallFrameSQLGetConnectOptionW : public CallFrame
{
public:
	struct Args
	{
		fXX(ConnectionHandle,		SQLHDBC)
		fXX(Option,					SQLUSMALLINT)
		fXX(Value,					SQLPOINTER)
	} args;
	Legend Option;
	CallFrameSQLGetConnectOptionW(const Args &rArgs)
	: CallFrame(SQL_API_SQLGETCONNECTOPTION, "SQLGetConnectOptionW"), args(rArgs)
	, Option(args.Option, Legend::SqlConnectOption)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(ConnectionHandle,		SQLHDBC)
			fHA(Option,					SQLUSMALLINT)
			fPX(Value,					SQLPOINTER)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLGetConnectOptionW(CallFrameSQLGetConnectOptionW::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLGetConnectOption ? stack->Push(new CallFrameSQLGetConnectOptionW(args)) : -1;
}

class CallFrameSQLSetStmtOption : public CallFrame
{
public:
	struct Args
	{
		fXX(StatementHandle,		SQLHSTMT)
		fXX(Option,					SQLUSMALLINT)
		fXX(Value,					SQLUINTEGER)
	} args;
	Legend Option;
	CallFrameSQLSetStmtOption(const Args &rArgs)
	: CallFrame(SQL_API_SQLSETSTMTOPTION, "SQLSetStmtOption"), args(rArgs)
	, Option(args.Option, Legend::SqlStmtOption)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(StatementHandle,		SQLHSTMT)
			fHA(Option,					SQLUSMALLINT)
			fGX(Value,					SQLUINTEGER)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLSetStmtOption(CallFrameSQLSetStmtOption::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLSetStmtOption ? stack->Push(new CallFrameSQLSetStmtOption(args)) : -1;
}

class CallFrameSQLGetStmtOption : public CallFrame
{
public:
	struct Args
	{
		fXX(StatementHandle,		SQLHSTMT)
		fXX(Option,					SQLUSMALLINT)
		fXX(Value,					SQLPOINTER)
	} args;
	Legend Option;
	CallFrameSQLGetStmtOption(const Args &rArgs)
	: CallFrame(SQL_API_SQLGETSTMTOPTION, "SQLGetStmtOption"), args(rArgs)
	, Option(args.Option, Legend::SqlStmtOption)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(StatementHandle,		SQLHSTMT)
			fHA(Option,					SQLUSMALLINT)
			fPX(Value,					SQLPOINTER)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLGetStmtOption(CallFrameSQLGetStmtOption::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLGetStmtOption ? stack->Push(new CallFrameSQLGetStmtOption(args)) : -1;
}

class CallFrameSQLSetEnvAttr : public CallFrame
{
public:
	struct Args
	{
		fXX(EnvironmentHandle,		SQLHENV)
		fXX(Attribute,				SQLINTEGER)
		fXX(ValuePtr,				SQLPOINTER)
		fXX(StringLength,			SQLINTEGER)
	} args;
	CallFrameSQLSetEnvAttr(const Args &rArgs)
	: CallFrame(SQL_API_SQLSETENVATTR, "SQLSetEnvAttr", SQL_HANDLE_ENV), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(EnvironmentHandle,		SQLHENV)
			fGX(Attribute,				SQLINTEGER)
			fPX(ValuePtr,				SQLPOINTER)
			fGX(StringLength,			SQLINTEGER)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLSetEnvAttr(CallFrameSQLSetEnvAttr::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLSetEnvAttr ? stack->Push(new CallFrameSQLSetEnvAttr(args)) : -1;
}

class CallFrameSQLGetEnvAttr : public CallFrame
{
public:
	struct Args
	{
		fXX(EnvironmentHandle,		SQLHENV)
		fXX(Attribute,				SQLINTEGER)
		fXX(ValuePtr,				SQLPOINTER)
		fXX(BufferLength,			SQLINTEGER)
		fXX(StringLengthPtr,		SQLINTEGER*)
	} args;
	CallFrameSQLGetEnvAttr(const Args &rArgs)
	: CallFrame(SQL_API_SQLGETENVATTR, "SQLGetEnvAttr", SQL_HANDLE_ENV), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(EnvironmentHandle,		SQLHENV)
			fGX(Attribute,				SQLINTEGER)
			fPX(ValuePtr,				SQLPOINTER)
			fGX(BufferLength,			SQLINTEGER)
			fPX(StringLengthPtr,		SQLINTEGER*)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLGetEnvAttr(CallFrameSQLGetEnvAttr::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLGetEnvAttr ? stack->Push(new CallFrameSQLGetEnvAttr(args)) : -1;
}

class CallFrameSQLSetConnectAttr : public CallFrame
{
public:
	struct Args
	{
		fXX(ConnectionHandle,		SQLHDBC)
		fXX(Attribute,				SQLINTEGER)
		fXX(ValuePtr,				SQLPOINTER)
		fXX(StringLength,			SQLINTEGER)
	} args;
	Legend Attribute;
	CallFrameSQLSetConnectAttr(const Args &rArgs)
	: CallFrame(SQL_API_SQLSETCONNECTATTR, "SQLSetConnectAttr", SQL_HANDLE_DBC), args(rArgs)
	, Attribute(args.Attribute, Legend::SqlConnectAttr)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(ConnectionHandle,		SQLHDBC)
			fHA(Attribute,				SQLINTEGER)
			fPX(ValuePtr,				SQLPOINTER)
			fGX(StringLength,			SQLINTEGER)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLSetConnectAttr(CallFrameSQLSetConnectAttr::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLSetConnectAttr ? stack->Push(new CallFrameSQLSetConnectAttr(args)) : -1;
}

class CallFrameSQLSetConnectAttrW : public CallFrame
{
public:
	struct Args
	{
		fXX(ConnectionHandle,		SQLHDBC)
		fXX(Attribute,				SQLINTEGER)
		fXX(ValuePtr,				SQLPOINTER)
		fXX(StringLength,			SQLINTEGER)
	} args;
	Legend Attribute;
	CallFrameSQLSetConnectAttrW(const Args &rArgs)
	: CallFrame(SQL_API_SQLSETCONNECTATTR, "SQLSetConnectAttrW", SQL_HANDLE_DBC), args(rArgs)
	, Attribute(args.Attribute, Legend::SqlConnectAttr)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(ConnectionHandle,		SQLHDBC)
			fHA(Attribute,				SQLINTEGER)
			fPX(ValuePtr,				SQLPOINTER)
			fGX(StringLength,			SQLINTEGER)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLSetConnectAttrW(CallFrameSQLSetConnectAttrW::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLSetConnectAttr ? stack->Push(new CallFrameSQLSetConnectAttrW(args)) : -1;
}

class CallFrameSQLGetConnectAttr : public CallFrame
{
public:
	struct Args
	{
		fXX(ConnectionHandle,		SQLHDBC)
		fXX(Attribute,				SQLINTEGER)
		fXX(ValuePtr,				SQLPOINTER)
		fXX(BufferLength,			SQLINTEGER)
		fXX(StringLengthPtr,		SQLINTEGER*)
	} args;
	Legend Attribute;
	CallFrameSQLGetConnectAttr(const Args &rArgs)
	: CallFrame(SQL_API_SQLGETCONNECTATTR, "SQLGetConnectAttr", SQL_HANDLE_DBC), args(rArgs)
	, Attribute(args.Attribute, Legend::SqlConnectAttr)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(ConnectionHandle,		SQLHDBC)
			fHA(Attribute,				SQLINTEGER)
			fPX(ValuePtr,				SQLPOINTER)
			fGX(BufferLength,			SQLINTEGER)
			fPX(StringLengthPtr,		SQLINTEGER*)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLGetConnectAttr(CallFrameSQLGetConnectAttr::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLGetConnectAttr ? stack->Push(new CallFrameSQLGetConnectAttr(args)) : -1;
}

class CallFrameSQLGetConnectAttrW : public CallFrame
{
public:
	struct Args
	{
		fXX(ConnectionHandle,		SQLHDBC)
		fXX(Attribute,				SQLINTEGER)
		fXX(ValuePtr,				SQLPOINTER)
		fXX(BufferLength,			SQLINTEGER)
		fXX(StringLengthPtr,		SQLINTEGER*)
	} args;
	Legend Attribute;
	CallFrameSQLGetConnectAttrW(const Args &rArgs)
	: CallFrame(SQL_API_SQLGETCONNECTATTR, "SQLGetConnectAttrW", SQL_HANDLE_DBC), args(rArgs)
	, Attribute(args.Attribute, Legend::SqlConnectAttr)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(ConnectionHandle,		SQLHDBC)
			fHA(Attribute,				SQLINTEGER)
			fPX(ValuePtr,				SQLPOINTER)
			fGX(BufferLength,			SQLINTEGER)
			fPX(StringLengthPtr,		SQLINTEGER*)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLGetConnectAttrW(CallFrameSQLGetConnectAttrW::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLGetConnectAttr ? stack->Push(new CallFrameSQLGetConnectAttrW(args)) : -1;
}

class CallFrameSQLSetStmtAttr : public CallFrame
{
public:
	struct Args
	{
		fXX(StatementHandle,		SQLHSTMT)
		fXX(Attribute,				SQLINTEGER)
		fXX(ValuePtr,				SQLPOINTER)
		fXX(StringLength,			SQLINTEGER)
	} args;
	Legend Attribute;
	CallFrameSQLSetStmtAttr(const Args &rArgs)
	: CallFrame(SQL_API_SQLSETSTMTATTR, "SQLSetStmtAttr", SQL_HANDLE_STMT), args(rArgs)
	, Attribute(args.Attribute, Legend::SqlStmtAttr)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(StatementHandle,		SQLHSTMT)
			fHA(Attribute,				SQLINTEGER)
			fPX(ValuePtr,				SQLPOINTER)
			fGX(StringLength,			SQLINTEGER)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLSetStmtAttr(CallFrameSQLSetStmtAttr::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLSetStmtAttr ? stack->Push(new CallFrameSQLSetStmtAttr(args)) : -1;
}

class CallFrameSQLSetStmtAttrW : public CallFrame
{
public:
	struct Args
	{
		fXX(StatementHandle,		SQLHSTMT)
		fXX(Attribute,				SQLINTEGER)
		fXX(ValuePtr,				SQLPOINTER)
		fXX(StringLength,			SQLINTEGER)
	} args;
	Legend Attribute;
	CallFrameSQLSetStmtAttrW(const Args &rArgs)
	: CallFrame(SQL_API_SQLSETSTMTATTR, "SQLSetStmtAttrW", SQL_HANDLE_STMT), args(rArgs)
	, Attribute(args.Attribute, Legend::SqlStmtAttr)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(StatementHandle,		SQLHSTMT)
			fHA(Attribute,				SQLINTEGER)
			fPX(ValuePtr,				SQLPOINTER)
			fGX(StringLength,			SQLINTEGER)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLSetStmtAttrW(CallFrameSQLSetStmtAttrW::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLSetStmtAttr ? stack->Push(new CallFrameSQLSetStmtAttrW(args)) : -1;
}

class CallFrameSQLGetStmtAttr : public CallFrame
{
public:
	struct Args
	{
		fXX(StatementHandle,		SQLHSTMT)
		fXX(Attribute,				SQLINTEGER)
		fXX(ValuePtr,				SQLPOINTER)
		fXX(BufferLength,			SQLINTEGER)
		fXX(StringLengthPtr,		SQLINTEGER*)
	} args;
	Legend Attribute;
	CallFrameSQLGetStmtAttr(const Args &rArgs)
	: CallFrame(SQL_API_SQLGETSTMTATTR, "SQLGetStmtAttr", SQL_HANDLE_STMT), args(rArgs)
	, Attribute(args.Attribute, Legend::SqlStmtAttr)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(StatementHandle,		SQLHSTMT)
			fHA(Attribute,				SQLINTEGER)
			fPX(ValuePtr,				SQLPOINTER)
			fGX(BufferLength,			SQLINTEGER)
			fPX(StringLengthPtr,		SQLINTEGER*)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLGetStmtAttr(CallFrameSQLGetStmtAttr::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLGetStmtAttr ? stack->Push(new CallFrameSQLGetStmtAttr(args)) : -1;
}

class CallFrameSQLGetStmtAttrW : public CallFrame
{
public:
	struct Args
	{
		fXX(StatementHandle,		SQLHSTMT)
		fXX(Attribute,				SQLINTEGER)
		fXX(ValuePtr,				SQLPOINTER)
		fXX(BufferLength,			SQLINTEGER)
		fXX(StringLengthPtr,		SQLINTEGER*)
	} args;
	Legend Attribute;
	CallFrameSQLGetStmtAttrW(const Args &rArgs)
	: CallFrame(SQL_API_SQLGETSTMTATTR, "SQLGetStmtAttrW", SQL_HANDLE_STMT), args(rArgs)
	, Attribute(args.Attribute, Legend::SqlStmtAttr)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(StatementHandle,		SQLHSTMT)
			fHA(Attribute,				SQLINTEGER)
			fPX(ValuePtr,				SQLPOINTER)
			fGX(BufferLength,			SQLINTEGER)
			fPX(StringLengthPtr,		SQLINTEGER*)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLGetStmtAttrW(CallFrameSQLGetStmtAttrW::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLGetStmtAttr ? stack->Push(new CallFrameSQLGetStmtAttrW(args)) : -1;
}

class CallFrameSQLPrepare : public CallFrame
{
public:
	struct Args
	{
		fXX(StatementHandle,		SQLHSTMT)
		fXX(StatementText,			SQLCHAR*)
		fXX(TextLength,				SQLINTEGER)
	} args;
	CallFrameSQLPrepare(const Args &rArgs)
	: CallFrame(SQL_API_SQLPREPARE, "SQLPrepare", SQL_HANDLE_STMT), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(StatementHandle,		SQLHSTMT)
			fHS(StatementText,			SQLCHAR*)
			fGX(TextLength,				SQLINTEGER)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLPrepare(CallFrameSQLPrepare::Args args)
{
	if (ODBCTrace.SQLExecute)
	{
		::EnterCriticalSection(&CallStack::cs);
		if (StatementInfo *pInfo = StatementInfo::FromHandle(args.StatementHandle))
		{
			pInfo->SetStatementText((LPCSTR)args.StatementText, args.TextLength);
		}
		::LeaveCriticalSection(&CallStack::cs);
	}
	return stack->HideLevel() < ODBCTrace.SQLPrepare ? stack->Push(new CallFrameSQLPrepare(args)) : -1;
}

class CallFrameSQLPrepareW : public CallFrame
{
public:
	struct Args
	{
		fXX(StatementHandle,		SQLHSTMT)
		fXX(StatementText,			SQLWCHAR*)
		fXX(TextLength,				SQLINTEGER)
	} args;
	CallFrameSQLPrepareW(const Args &rArgs)
	: CallFrame(SQL_API_SQLPREPARE, "SQLPrepareW", SQL_HANDLE_STMT), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(StatementHandle,		SQLHSTMT)
			fLS(StatementText,			SQLWCHAR*)
			fGX(TextLength,				SQLINTEGER)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLPrepareW(CallFrameSQLPrepareW::Args args)
{
	if (ODBCTrace.SQLExecute)
	{
		::EnterCriticalSection(&CallStack::cs);
		if (StatementInfo *pInfo = StatementInfo::FromHandle(args.StatementHandle))
		{
			pInfo->SetStatementText((LPCWSTR)args.StatementText, args.TextLength);
		}
		::LeaveCriticalSection(&CallStack::cs);
	}
	return stack->HideLevel() < ODBCTrace.SQLPrepare ? stack->Push(new CallFrameSQLPrepareW(args)) : -1;
}

class CallFrameSQLBindParameter : public CallFrame
{
public:
	struct Args
	{
		fXX(StatementHandle,		SQLHSTMT)
		fXX(ParameterNumber,		SQLUSMALLINT)
		fXX(InputOutputType,		SQLSMALLINT)
		fXX(ValueType,				SQLSMALLINT)
		fXX(ParameterType,			SQLSMALLINT)
		fXX(ColumnSize,				SQLUINTEGER)
		fXX(DecimalDigits,			SQLSMALLINT)
		fXX(ParameterValuePtr,		SQLPOINTER)
		fXX(BufferLength,			SQLINTEGER)
		fXX(StrLen_or_IndPtr,		SQLINTEGER*)
	} args;
	Legend InputOutputType, ValueType, ParameterType;
	CallFrameSQLBindParameter(const Args &rArgs)
	: CallFrame(SQL_API_SQLBINDPARAMETER, "SQLBindParameter", SQL_HANDLE_STMT), args(rArgs)
	, InputOutputType	(args.InputOutputType,	Legend::SqlInputOutputType)
	, ValueType			(args.ValueType,		Legend::SqlCType)
	, ParameterType		(args.ParameterType,	Legend::SqlType)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(StatementHandle,		SQLHSTMT)
			fHX(ParameterNumber,		SQLUSMALLINT)
			fHA(InputOutputType,		SQLSMALLINT)
			fHA(ValueType,				SQLSMALLINT)
			fHA(ParameterType,			SQLSMALLINT)
			fGX(ColumnSize,				SQLUINTEGER)
			fHX(DecimalDigits,			SQLSMALLINT)
			fPX(ParameterValuePtr,		SQLPOINTER)
			fGX(BufferLength,			SQLINTEGER)
			fPX(StrLen_or_IndPtr,		SQLINTEGER*)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLBindParameter(CallFrameSQLBindParameter::Args args)
{
	if (stack->depth < RETCODE(ODBCTrace.SQLExecute | ODBCTrace.SQLExecDirect))
	{
		::EnterCriticalSection(&CallStack::cs);
		if (StatementInfo *pInfo = StatementInfo::FromHandle(args.StatementHandle, args.ParameterNumber))
		{
			if (StatementInfo::Param *pParam = pInfo->GetParam(args.ParameterNumber))
			{
				pParam->InputOutputType		= args.InputOutputType;
				pParam->ValueType			= args.ValueType;
				pParam->ParameterType		= args.ParameterType;
				pParam->ColumnSize			= args.ColumnSize;
				pParam->DecimalDigits		= args.DecimalDigits;
				pParam->ParameterValuePtr	= args.ParameterValuePtr;
				pParam->BufferLength		= args.BufferLength;
				pParam->StrLen_or_IndPtr	= args.StrLen_or_IndPtr;
			}
		}
		::LeaveCriticalSection(&CallStack::cs);
	}
	return stack->HideLevel() < ODBCTrace.SQLBindParameter ? stack->Push(new CallFrameSQLBindParameter(args)) : -1;
}

class CallFrameSQLBindParam : public CallFrame
{
public:
	struct Args
	{
		fXX(StatementHandle,		SQLHSTMT)
		fXX(ParameterNumber,		SQLUSMALLINT)
		fXX(ValueType,				SQLSMALLINT)
		fXX(ParameterType,			SQLSMALLINT)
		fXX(ColumnSize,				SQLUINTEGER)
		fXX(DecimalDigits,			SQLSMALLINT)
		fXX(ParameterValuePtr,		SQLPOINTER)
		fXX(StrLen_or_IndPtr,		SQLINTEGER*)
	} args;
	CallFrameSQLBindParam(const Args &rArgs)
	: CallFrame(SQL_API_SQLBINDPARAM, "SQLBindParam", SQL_HANDLE_STMT), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(StatementHandle,		SQLHSTMT)
			fHX(ParameterNumber,		SQLUSMALLINT)
			fHX(ValueType,				SQLSMALLINT)
			fHX(ParameterType,			SQLSMALLINT)
			fGX(ColumnSize,				SQLUINTEGER)
			fHX(DecimalDigits,			SQLSMALLINT)
			fPX(ParameterValuePtr,		SQLPOINTER)
			fPX(StrLen_or_IndPtr,		SQLINTEGER*)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLBindParam(CallFrameSQLBindParam::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLBindParam ? stack->Push(new CallFrameSQLBindParam(args)) : -1;
}

class CallFrameSQLSetParam : public CallFrame
{
public:
	struct Args
	{
		fXX(StatementHandle,		SQLHSTMT)
		fXX(ParameterNumber,		SQLUSMALLINT)
		fXX(ValueType,				SQLSMALLINT)
		fXX(ParameterType,			SQLSMALLINT)
		fXX(ColumnSize,				SQLUINTEGER)
		fXX(DecimalDigits,			SQLSMALLINT)
		fXX(ParameterValuePtr,		SQLPOINTER)
		fXX(StrLen_or_IndPtr,		SQLINTEGER*)
	} args;
	CallFrameSQLSetParam(const Args &rArgs)
	: CallFrame(SQL_API_SQLSETPARAM, "SQLSetParam", SQL_HANDLE_STMT), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(StatementHandle,		SQLHSTMT)
			fHX(ParameterNumber,		SQLUSMALLINT)
			fHX(ValueType,				SQLSMALLINT)
			fHX(ParameterType,			SQLSMALLINT)
			fGX(ColumnSize,				SQLUINTEGER)
			fHX(DecimalDigits,			SQLSMALLINT)
			fPX(ParameterValuePtr,		SQLPOINTER)
			fPX(StrLen_or_IndPtr,		SQLINTEGER*)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLSetParam(CallFrameSQLSetParam::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLSetParam ? stack->Push(new CallFrameSQLSetParam(args)) : -1;
}

class CallFrameSQLDescribeParam : public CallFrame
{
public:
	struct Args
	{
		fXX(StatementHandle,		SQLHSTMT)
		fXX(ParameterNumber,		SQLUSMALLINT)
		fXX(DataTypePtr,			SQLSMALLINT*)
		fXX(ParameterSizePtr,		SQLINTEGER*)
		fXX(DecimalDigitsPtr,		SQLSMALLINT*)
		fXX(NullablePtr,			SQLSMALLINT*)
	} args;
	CallFrameSQLDescribeParam(const Args &rArgs)
	: CallFrame(SQL_API_SQLDESCRIBEPARAM, "SQLDescribeParam", SQL_HANDLE_STMT), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(StatementHandle,		SQLHSTMT)
			fHX(ParameterNumber,		SQLUSMALLINT)
			fPX(DataTypePtr,			SQLSMALLINT*)
			fPX(ParameterSizePtr,		SQLINTEGER*)
			fPX(DecimalDigitsPtr,		SQLSMALLINT*)
			fPX(NullablePtr,			SQLSMALLINT*)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLDescribeParam(CallFrameSQLDescribeParam::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLDescribeParam ? stack->Push(new CallFrameSQLDescribeParam(args)) : -1;
}

class CallFrameSQLParamOptions : public CallFrame
{
public:
	struct Args
	{
		fXX(hstmt,					SQLHSTMT)
		fXX(crow,					SQLUINTEGER)
		fXX(pirow,					SQLUINTEGER*)
	} args;
	CallFrameSQLParamOptions(const Args &rArgs)
	: CallFrame(SQL_API_SQLPARAMOPTIONS, "SQLParamOptions", SQL_HANDLE_STMT), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(hstmt,					SQLHSTMT)
			fGX(crow,					SQLUINTEGER)
			fPX(pirow,					SQLUINTEGER*)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLParamOptions(CallFrameSQLParamOptions::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLParamOptions ? stack->Push(new CallFrameSQLParamOptions(args)) : -1;
}

class CallFrameSQLNumParams : public CallFrame
{
public:
	struct Args
	{
		fXX(StatementHandle,		SQLHSTMT)
		fXX(ParameterCountPtr,		SQLSMALLINT*)
	} args;
	CallFrameSQLNumParams(const Args &rArgs)
	: CallFrame(SQL_API_SQLNUMPARAMS, "SQLNumParams", SQL_HANDLE_STMT), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(StatementHandle,		SQLHSTMT)
			fPX(ParameterCountPtr,		SQLSMALLINT*)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLNumParams(CallFrameSQLNumParams::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLNumParams ? stack->Push(new CallFrameSQLNumParams(args)) : -1;
}

class CallFrameSQLSetScrollOptions : public CallFrame
{
public:
	struct Args
	{
		fXX(hstmt,					SQLHSTMT)
		fXX(fConcurrency,			SQLUSMALLINT)
		fXX(crowKeyset,				SQLINTEGER)
		fXX(crowRowset,				SQLUSMALLINT)
	} args;
	CallFrameSQLSetScrollOptions(const Args &rArgs)
	: CallFrame(SQL_API_SQLSETSCROLLOPTIONS, "SQLSetScrollOptions", SQL_HANDLE_STMT), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(hstmt,					SQLHSTMT)
			fHX(fConcurrency,			SQLUSMALLINT)
			fGX(crowKeyset,				SQLINTEGER)
			fHX(crowRowset,				SQLUSMALLINT)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLSetScrollOptions(CallFrameSQLSetScrollOptions::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLSetScrollOptions ? stack->Push(new CallFrameSQLSetScrollOptions(args)) : -1;
}

class CallFrameSQLExecute : public CallFrame
{
public:
	struct Args
	{
		fXX(StatementHandle,		SQLHSTMT)
	} args;
	CallFrameSQLExecute(const Args &rArgs)
	: CallFrame(SQL_API_SQLEXECUTE, "SQLExecute", SQL_HANDLE_STMT), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(StatementHandle,		SQLHSTMT)
		);
		CallFrame::Dump(fmt);
	}
	virtual char *DumpDiagFields(RETCODE)
	{
		//RETCODE result = ::SQLGetDiagField(SQL_HANDLE_STMT, args.StatementHandle, 0, SQL_DIAG_DYNAMIC_FUNCTION, pszText, cchTextMax, &cchText);
		return StatementInfo::BuildSQL(args.StatementHandle);
	}
};

RETCODE SQL_API TraceSQLExecute(CallFrameSQLExecute::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLExecute ? stack->Push(new CallFrameSQLExecute(args)) : -1;
}

class CallFrameSQLExecDirect : public CallFrame
{
public:
	struct Args
	{
		fXX(StatementHandle,		SQLHSTMT)
		fXX(StatementText,			SQLCHAR*)
		fXX(TextLength,				SQLINTEGER)
	} args;
	CallFrameSQLExecDirect(const Args &rArgs)
	: CallFrame(SQL_API_SQLEXECDIRECT, "SQLExecDirect", SQL_HANDLE_STMT), args(rArgs)
	{
		::EnterCriticalSection(&CallStack::cs);
		if (StatementInfo *pInfo = StatementInfo::FromHandle(args.StatementHandle))
		{
			pInfo->SetStatementText((LPCSTR)args.StatementText, args.TextLength);
		}
		::LeaveCriticalSection(&CallStack::cs);
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(StatementHandle,		SQLHSTMT)
			fHS(StatementText,			SQLCHAR*)
			fGX(TextLength,				SQLINTEGER)
		);
		CallFrame::Dump(fmt);
		/*if (char *pszText = StatementInfo::BuildSQL(args.StatementHandle))
		{
			TraceRaw(CTraceWnd::rgbSqlEx, pszText);
			GlobalFreePtr(pszText);
		}*/
	}
	virtual char *DumpDiagFields(RETCODE)
	{
		return StatementInfo::BuildSQL(args.StatementHandle);
	}
};

RETCODE SQL_API TraceSQLExecDirect(CallFrameSQLExecDirect::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLExecDirect ? stack->Push(new CallFrameSQLExecDirect(args)) : -1;
}

class CallFrameSQLExecDirectW : public CallFrame
{
public:
	struct Args
	{
		fXX(StatementHandle,		SQLHSTMT)
		fXX(StatementText,			SQLWCHAR*)
		fXX(TextLength,				SQLINTEGER)
	} args;
	CallFrameSQLExecDirectW(const Args &rArgs)
	: CallFrame(SQL_API_SQLEXECDIRECT, "SQLExecDirectW", SQL_HANDLE_STMT), args(rArgs)
	{
		::EnterCriticalSection(&CallStack::cs);
		if (StatementInfo *pInfo = StatementInfo::FromHandle(args.StatementHandle))
		{
			pInfo->SetStatementText((LPCWSTR)args.StatementText, args.TextLength);
		}
		::LeaveCriticalSection(&CallStack::cs);
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(StatementHandle,		SQLHSTMT)
			fLS(StatementText,			SQLWCHAR*)
			fGX(TextLength,				SQLINTEGER)
		);
		CallFrame::Dump(fmt);
	}
	virtual char *DumpDiagFields(RETCODE)
	{
		return StatementInfo::BuildSQL(args.StatementHandle);
	}
};

RETCODE SQL_API TraceSQLExecDirectW(CallFrameSQLExecDirectW::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLExecDirect ? stack->Push(new CallFrameSQLExecDirectW(args)) : -1;
}

class CallFrameSQLNativeSql : public CallFrame
{
public:
	struct Args
	{
		fXX(ConnectionHandle,		SQLHDBC)
		fXX(InStatementText,		SQLCHAR*)
		fXX(TextLength1,			SQLINTEGER)
		fXX(OutStatementText,		SQLCHAR*)
		fXX(BufferLength,			SQLINTEGER)
		fXX(TextLength2Ptr,			SQLINTEGER*)
	} args;
	CallFrameSQLNativeSql(const Args &rArgs)
	: CallFrame(SQL_API_SQLNATIVESQL, "SQLNativeSql", SQL_HANDLE_DBC), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(ConnectionHandle,		SQLHDBC)
			fHS(InStatementText,		SQLCHAR*)
			fGX(TextLength1,			SQLINTEGER)
			fPX(OutStatementText,		SQLCHAR*)
			fGX(BufferLength,			SQLINTEGER)
			fPX(TextLength2Ptr,			SQLINTEGER*)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLNativeSql(CallFrameSQLNativeSql::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLNativeSql ? stack->Push(new CallFrameSQLNativeSql(args)) : -1;
}

class CallFrameSQLNativeSqlW : public CallFrame
{
public:
	struct Args
	{
		fXX(ConnectionHandle,		SQLHDBC)
		fXX(InStatementText,		SQLWCHAR*)
		fXX(TextLength1,			SQLINTEGER)
		fXX(OutStatementText,		SQLWCHAR*)
		fXX(BufferLength,			SQLINTEGER)
		fXX(TextLength2Ptr,			SQLINTEGER*)
	} args;
	CallFrameSQLNativeSqlW(const Args &rArgs)
	: CallFrame(SQL_API_SQLNATIVESQL, "SQLNativeSqlW", SQL_HANDLE_DBC), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(ConnectionHandle,		SQLHDBC)
			fLS(InStatementText,		SQLWCHAR*)
			fGX(TextLength1,			SQLINTEGER)
			fPX(OutStatementText,		SQLWCHAR*)
			fGX(BufferLength,			SQLINTEGER)
			fPX(TextLength2Ptr,			SQLINTEGER*)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLNativeSqlW(CallFrameSQLNativeSqlW::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLNativeSql ? stack->Push(new CallFrameSQLNativeSqlW(args)) : -1;
}

class CallFrameSQLParamData : public CallFrame
{
public:
	struct Args
	{
		fXX(StatementHandle,		SQLHSTMT)
		fXX(ValuePtrPtr,			SQLPOINTER*)
	} args;
	CallFrameSQLParamData(const Args &rArgs)
	: CallFrame(SQL_API_SQLPARAMDATA, "SQLParamData", SQL_HANDLE_STMT), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(StatementHandle,		SQLHSTMT)
			fPX(ValuePtrPtr,			SQLPOINTER*)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLParamData(CallFrameSQLParamData::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLParamData ? stack->Push(new CallFrameSQLParamData(args)) : -1;
}

class CallFrameSQLPutData : public CallFrame
{
public:
	struct Args
	{
		fXX(StatementHandle,		SQLHSTMT)
		fXX(DataPtr,				SQLPOINTER)
		fXX(StrLen_or_Ind,			SQLINTEGER)
	} args;
	CallFrameSQLPutData(const Args &rArgs)
	: CallFrame(SQL_API_SQLPUTDATA, "SQLPutData", SQL_HANDLE_STMT), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(StatementHandle,		SQLHSTMT)
			fPX(DataPtr,				SQLPOINTER)
			fGX(StrLen_or_Ind,			SQLINTEGER)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLPutData(CallFrameSQLPutData::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLPutData ? stack->Push(new CallFrameSQLPutData(args)) : -1;
}

class CallFrameSQLCancel : public CallFrame
{
public:
	struct Args
	{
		fXX(StatementHandle,		SQLHSTMT)
	} args;
	CallFrameSQLCancel(const Args &rArgs)
	: CallFrame(SQL_API_SQLCANCEL, "SQLCancel", SQL_HANDLE_STMT), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(StatementHandle,		SQLHSTMT)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLCancel(CallFrameSQLCancel::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLCancel ? stack->Push(new CallFrameSQLCancel(args)) : -1;
}

class CallFrameSQLAllocEnv : public CallFrame
{
public:
	struct Args
	{
		fXX(EnvironmentHandle,		SQLHENV*)
	} args;
	CallFrameSQLAllocEnv(const Args &rArgs)
	: CallFrame(SQL_API_SQLALLOCENV, "SQLAllocEnv", SQL_HANDLE_ENV), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fPX(EnvironmentHandle,		SQLHENV*)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLAllocEnv(CallFrameSQLAllocEnv::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLAllocEnv ? stack->Push(new CallFrameSQLAllocEnv(args)) : -1;
}

class CallFrameSQLDataSources : public CallFrame
{
public:
	struct Args
	{
		fXX(EnvironmentHandle,		SQLHENV)
		fXX(Direction,				SQLUSMALLINT)
		fXX(ServerName,				SQLCHAR*)
		fXX(BufferLength1,			SQLSMALLINT)
		fXX(NameLength1Ptr,			SQLSMALLINT*)
		fXX(Description,			SQLCHAR*)
		fXX(BufferLength2,			SQLSMALLINT)
		fXX(NameLength2Ptr,			SQLSMALLINT*)
	} args;
	CallFrameSQLDataSources(const Args &rArgs)
	: CallFrame(SQL_API_SQLDATASOURCES, "SQLDataSources", SQL_HANDLE_ENV), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(EnvironmentHandle,		SQLHENV)
			fHX(Direction,				SQLUSMALLINT)
			fPX(ServerName,				SQLCHAR*)
			fHX(BufferLength1,			SQLSMALLINT)
			fPX(NameLength1Ptr,			SQLSMALLINT*)
			fPX(Description,			SQLCHAR*)
			fHX(BufferLength2,			SQLSMALLINT)
			fPX(NameLength2Ptr,			SQLSMALLINT*)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLDataSources(CallFrameSQLDataSources::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLDataSources ? stack->Push(new CallFrameSQLDataSources(args)) : -1;
}

class CallFrameSQLDataSourcesW : public CallFrame
{
public:
	struct Args
	{
		fXX(EnvironmentHandle,		SQLHENV)
		fXX(Direction,				SQLUSMALLINT)
		fXX(ServerName,				SQLWCHAR*)
		fXX(BufferLength1,			SQLSMALLINT)
		fXX(NameLength1Ptr,			SQLSMALLINT*)
		fXX(Description,			SQLWCHAR*)
		fXX(BufferLength2,			SQLSMALLINT)
		fXX(NameLength2Ptr,			SQLSMALLINT*)
	} args;
	CallFrameSQLDataSourcesW(const Args &rArgs)
	: CallFrame(SQL_API_SQLDATASOURCES, "SQLDataSourcesW", SQL_HANDLE_ENV), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(EnvironmentHandle,		SQLHENV)
			fHX(Direction,				SQLUSMALLINT)
			fPX(ServerName,				SQLWCHAR*)
			fHX(BufferLength1,			SQLSMALLINT)
			fPX(NameLength1Ptr,			SQLSMALLINT*)
			fPX(Description,			SQLWCHAR*)
			fHX(BufferLength2,			SQLSMALLINT)
			fPX(NameLength2Ptr,			SQLSMALLINT*)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLDataSourcesW(CallFrameSQLDataSourcesW::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLDataSources ? stack->Push(new CallFrameSQLDataSourcesW(args)) : -1;
}

class CallFrameSQLDrivers : public CallFrame
{
public:
	struct Args
	{
		fXX(EnvironmentHandle,		SQLHENV)
		fXX(Direction,				SQLUSMALLINT)
		fXX(DriverDescription,		SQLCHAR*)
		fXX(BufferLength1,			SQLSMALLINT)
		fXX(NameLength1Ptr,			SQLSMALLINT*)
		fXX(DriverAttributes,		SQLCHAR*)
		fXX(BufferLength2,			SQLSMALLINT)
		fXX(NameLength2Ptr,			SQLSMALLINT*)
	} args;
	CallFrameSQLDrivers(const Args &rArgs)
	: CallFrame(SQL_API_SQLDATASOURCES, "SQLDrivers", SQL_HANDLE_ENV), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(EnvironmentHandle,		SQLHENV)
			fHX(Direction,				SQLUSMALLINT)
			fPX(DriverDescription,		SQLCHAR*)
			fHX(BufferLength1,			SQLSMALLINT)
			fPX(NameLength1Ptr,			SQLSMALLINT*)
			fPX(DriverAttributes,		SQLCHAR*)
			fHX(BufferLength2,			SQLSMALLINT)
			fPX(NameLength2Ptr,			SQLSMALLINT*)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLDrivers(CallFrameSQLDrivers::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLDrivers ? stack->Push(new CallFrameSQLDrivers(args)) : -1;
}

class CallFrameSQLDriversW : public CallFrame
{
public:
	struct Args
	{
		fXX(EnvironmentHandle,		SQLHENV)
		fXX(Direction,				SQLUSMALLINT)
		fXX(DriverDescription,		SQLWCHAR*)
		fXX(BufferLength1,			SQLSMALLINT)
		fXX(NameLength1Ptr,			SQLSMALLINT*)
		fXX(DriverAttributes,		SQLWCHAR*)
		fXX(BufferLength2,			SQLSMALLINT)
		fXX(NameLength2Ptr,			SQLSMALLINT*)
	} args;
	CallFrameSQLDriversW(const Args &rArgs)
	: CallFrame(SQL_API_SQLDATASOURCES, "SQLDriversW", SQL_HANDLE_ENV), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(EnvironmentHandle,		SQLHENV)
			fHX(Direction,				SQLUSMALLINT)
			fPX(DriverDescription,		SQLWCHAR*)
			fHX(BufferLength1,			SQLSMALLINT)
			fPX(NameLength1Ptr,			SQLSMALLINT*)
			fPX(DriverAttributes,		SQLWCHAR*)
			fHX(BufferLength2,			SQLSMALLINT)
			fPX(NameLength2Ptr,			SQLSMALLINT*)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE  SQL_API TraceSQLDriversW(CallFrameSQLDriversW::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLDrivers ? stack->Push(new CallFrameSQLDriversW(args)) : -1;
}

class CallFrameSQLGetDescField : public CallFrame
{
public:
	struct Args
	{
		fXX(DescriptorHandle,		SQLHDESC)
		fXX(RecNumber,				SQLSMALLINT)
		fXX(FieldIdentifier,		SQLSMALLINT)
		fXX(ValuePtr,				SQLPOINTER)
		fXX(BufferLength,			SQLINTEGER)
		fXX(StringLengthPtr,		SQLINTEGER*)
	} args;
	CallFrameSQLGetDescField(const Args &rArgs)
	: CallFrame(SQL_API_SQLGETDESCFIELD, "SQLGetDescField", SQL_HANDLE_DESC), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(DescriptorHandle,		SQLHDESC)
			fHX(RecNumber,				SQLSMALLINT)
			fHX(FieldIdentifier,		SQLSMALLINT)
			fPX(ValuePtr,				SQLPOINTER)
			fGX(BufferLength,			SQLINTEGER)
			fPX(StringLengthPtr,		SQLINTEGER*)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLGetDescField(CallFrameSQLGetDescField::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLGetDescField ? stack->Push(new CallFrameSQLGetDescField(args)) : -1;
}

class CallFrameSQLGetDescFieldW : public CallFrame
{
public:
	struct Args
	{
		fXX(DescriptorHandle,		SQLHDESC)
		fXX(RecNumber,				SQLSMALLINT)
		fXX(FieldIdentifier,		SQLSMALLINT)
		fXX(ValuePtr,				SQLPOINTER)
		fXX(BufferLength,			SQLINTEGER)
		fXX(StringLengthPtr,		SQLINTEGER*)
	} args;
	CallFrameSQLGetDescFieldW(const Args &rArgs)
	: CallFrame(SQL_API_SQLGETDESCFIELD, "SQLGetDescFieldW", SQL_HANDLE_DESC), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(DescriptorHandle,		SQLHDESC)
			fHX(RecNumber,				SQLSMALLINT)
			fHX(FieldIdentifier,		SQLSMALLINT)
			fPX(ValuePtr,				SQLPOINTER)
			fGX(BufferLength,			SQLINTEGER)
			fPX(StringLengthPtr,		SQLINTEGER*)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLGetDescFieldW(CallFrameSQLGetDescFieldW::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLGetDescField ? stack->Push(new CallFrameSQLGetDescFieldW(args)) : -1;
}

class CallFrameSQLSetDescField : public CallFrame
{
public:
	struct Args
	{
		fXX(DescriptorHandle,		SQLHDESC)
		fXX(RecNumber,				SQLSMALLINT)
		fXX(FieldIdentifier,		SQLSMALLINT)
		fXX(ValuePtr,				SQLPOINTER)
		fXX(BufferLength,			SQLINTEGER)
	} args;
	CallFrameSQLSetDescField(const Args &rArgs)
	: CallFrame(SQL_API_SQLSETDESCFIELD, "SQLSetDescField", SQL_HANDLE_DESC), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(DescriptorHandle,		SQLHDESC)
			fHX(RecNumber,				SQLSMALLINT)
			fHX(FieldIdentifier,		SQLSMALLINT)
			fPX(ValuePtr,				SQLPOINTER)
			fGX(BufferLength,			SQLINTEGER)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLSetDescField(CallFrameSQLSetDescField::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLSetDescField ? stack->Push(new CallFrameSQLSetDescField(args)) : -1;
}

class CallFrameSQLSetDescFieldW : public CallFrame
{
public:
	struct Args
	{
		fXX(DescriptorHandle,		SQLHDESC)
		fXX(RecNumber,				SQLSMALLINT)
		fXX(FieldIdentifier,		SQLSMALLINT)
		fXX(ValuePtr,				SQLPOINTER)
		fXX(BufferLength,			SQLINTEGER)
	} args;
	CallFrameSQLSetDescFieldW(const Args &rArgs)
	: CallFrame(SQL_API_SQLSETDESCFIELD, "SQLSetDescFieldW", SQL_HANDLE_DESC), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(DescriptorHandle,		SQLHDESC)
			fHX(RecNumber,				SQLSMALLINT)
			fHX(FieldIdentifier,		SQLSMALLINT)
			fPX(ValuePtr,				SQLPOINTER)
			fGX(BufferLength,			SQLINTEGER)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLSetDescFieldW(CallFrameSQLSetDescFieldW::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLSetDescField ? stack->Push(new CallFrameSQLSetDescFieldW(args)) : -1;
}

class CallFrameSQLGetDescRec : public CallFrame
{
public:
	struct Args
	{
	 	fXX(DescriptorHandle,		SQLHDESC)
		fXX(RecNumber,				SQLSMALLINT)
		fXX(Name,					SQLCHAR*)
		fXX(BufferLength,			SQLSMALLINT)
		fXX(StringLengthPtr,		SQLSMALLINT*)
		fXX(TypePtr,				SQLSMALLINT*)
		fXX(SubTypePtr,				SQLSMALLINT*)
		fXX(LengthPtr,				SQLINTEGER*)
		fXX(PrecisionPtr,			SQLSMALLINT*)
		fXX(ScalePtr,				SQLSMALLINT*)
		fXX(NullablePtr,			SQLSMALLINT*)
	} args;
	CallFrameSQLGetDescRec(const Args &rArgs)
	: CallFrame(SQL_API_SQLGETDESCREC, "SQLGetDescRec", SQL_HANDLE_DESC), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(DescriptorHandle,		SQLHDESC)
			fHX(RecNumber,				SQLSMALLINT)
			fHS(Name,					SQLCHAR*)
			fHX(BufferLength,			SQLSMALLINT)
			fPX(StringLengthPtr,		SQLSMALLINT*)
			fPX(TypePtr,				SQLSMALLINT*)
			fPX(SubTypePtr,				SQLSMALLINT*)
			fPX(LengthPtr,				SQLINTEGER*)
			fPX(PrecisionPtr,			SQLSMALLINT*)
			fPX(ScalePtr,				SQLSMALLINT*)
			fPX(NullablePtr,			SQLSMALLINT*)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLGetDescRec(CallFrameSQLGetDescRec::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLGetDescRec ? stack->Push(new CallFrameSQLGetDescRec(args)) : -1;
}

class CallFrameSQLGetDescRecW : public CallFrame
{
public:
	struct Args
	{
	 	fXX(DescriptorHandle,		SQLHDESC)
		fXX(RecNumber,				SQLSMALLINT)
		fXX(Name,					SQLWCHAR*)
		fXX(BufferLength,			SQLSMALLINT)
		fXX(StringLengthPtr,		SQLSMALLINT*)
		fXX(TypePtr,				SQLSMALLINT*)
		fXX(SubTypePtr,				SQLSMALLINT*)
		fXX(LengthPtr,				SQLINTEGER*)
		fXX(PrecisionPtr,			SQLSMALLINT*)
		fXX(ScalePtr,				SQLSMALLINT*)
		fXX(NullablePtr,			SQLSMALLINT*)
	} args;
	CallFrameSQLGetDescRecW(const Args &rArgs)
	: CallFrame(SQL_API_SQLGETDESCREC, "SQLGetDescRecW", SQL_HANDLE_DESC), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(DescriptorHandle,		SQLHDESC)
			fHX(RecNumber,				SQLSMALLINT)
			fLS(Name,					SQLWCHAR*)
			fHX(BufferLength,			SQLSMALLINT)
			fPX(StringLengthPtr,		SQLSMALLINT*)
			fPX(TypePtr,				SQLSMALLINT*)
			fPX(SubTypePtr,				SQLSMALLINT*)
			fPX(LengthPtr,				SQLINTEGER*)
			fPX(PrecisionPtr,			SQLSMALLINT*)
			fPX(ScalePtr,				SQLSMALLINT*)
			fPX(NullablePtr,			SQLSMALLINT*)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLGetDescRecW(CallFrameSQLGetDescRecW::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLGetDescRec ? stack->Push(new CallFrameSQLGetDescRecW(args)) : -1;
}

class CallFrameSQLSetDescRec : public CallFrame
{
public:
	struct Args
	{
		fXX(DescriptorHandle,		SQLHDESC)
		fXX(RecNumber,				SQLSMALLINT)
		fXX(Type,					SQLSMALLINT)
		fXX(SubType,				SQLSMALLINT)
		fXX(Length,					SQLINTEGER)
		fXX(Precision,				SQLSMALLINT)
		fXX(Scale,					SQLSMALLINT)
		fXX(DataPtr,				SQLPOINTER)
		fXX(StringLengthPtr,		SQLINTEGER*)
		fXX(IndicatorPtr,			SQLINTEGER*)
	} args;
	CallFrameSQLSetDescRec(const Args &rArgs)
	: CallFrame(SQL_API_SQLSETDESCREC, "SQLSetDescRec", SQL_HANDLE_DESC), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(DescriptorHandle,		SQLHDESC)
			fHX(RecNumber,				SQLSMALLINT)
			fHX(Type,					SQLSMALLINT)
			fHX(SubType,				SQLSMALLINT)
			fGX(Length,					SQLINTEGER)
			fHX(Precision,				SQLSMALLINT)
			fHX(Scale,					SQLSMALLINT)
			fPX(DataPtr,				SQLPOINTER)
			fPX(StringLengthPtr,		SQLINTEGER*)
			fPX(IndicatorPtr,			SQLINTEGER*)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLSetDescRec(CallFrameSQLSetDescRec::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLSetDescRec ? stack->Push(new CallFrameSQLSetDescRec(args)) : -1;
}

class CallFrameSQLAllocHandleStd : public CallFrame
{
public:
	struct Args
	{
		fXX(HandleType,				SQLSMALLINT)
		fXX(InputHandle,			SQLHANDLE)
		fXX(OutputHandlePtr,		SQLHANDLE*)
	} args;
	CallFrameSQLAllocHandleStd(const Args &rArgs)
	: CallFrame(SQL_API_SQLALLOCHANDLESTD, "SQLAllocHandleStd"), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fHX(HandleType,				SQLSMALLINT)
			fGX(InputHandle,			SQLHANDLE)
			fPX(OutputHandlePtr,		SQLHANDLE*)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLAllocHandleStd(CallFrameSQLAllocHandleStd::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLAllocHandleStd ? stack->Push(new CallFrameSQLAllocHandleStd(args)) : -1;
}

class CallFrameSQLAllocHandleStdW : public CallFrame
{
public:
	struct Args
	{
		fXX(HandleType,				SQLSMALLINT)
		fXX(InputHandle,			SQLHANDLE)
		fXX(OutputHandlePtr,		SQLHANDLE*)
	} args;
	CallFrameSQLAllocHandleStdW(const Args &rArgs)
	: CallFrame(SQL_API_SQLALLOCHANDLESTD, "SQLAllocHandleStdW"), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fHX(HandleType,				SQLSMALLINT)
			fGX(InputHandle,			SQLHANDLE)
			fPX(OutputHandlePtr,		SQLHANDLE*)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLAllocHandleStdW(CallFrameSQLAllocHandleStdW::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLAllocHandleStd ? stack->Push(new CallFrameSQLAllocHandleStdW(args)) : -1;
}

class CallFrameSQLFreeEnv : public CallFrame
{
public:
	struct Args
	{
		fXX(EnvironmentHandle,		SQLHENV)
	} args;
	CallFrameSQLFreeEnv(const Args &rArgs)
	: CallFrame(SQL_API_SQLFREEENV, "SQLFreeEnv", SQL_HANDLE_ENV), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(EnvironmentHandle,		SQLHENV)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLFreeEnv(CallFrameSQLFreeEnv::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLFreeEnv ? stack->Push(new CallFrameSQLFreeEnv(args)) : -1;
}

class CallFrameSQLAllocConnect : public CallFrame
{
public:
	struct Args
	{
		fXX(EnvironmentHandle,		SQLHENV)
		fXX(ConnectionHandle,		SQLHDBC*)
	} args;
	CallFrameSQLAllocConnect(const Args &rArgs)
	: CallFrame(SQL_API_SQLALLOCCONNECT, "SQLAllocConnect", SQL_HANDLE_ENV), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(EnvironmentHandle,		SQLHENV)
			fPX(ConnectionHandle,		SQLHDBC*)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLAllocConnect(CallFrameSQLAllocConnect::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLAllocConnect ? stack->Push(new CallFrameSQLAllocConnect(args)) : -1;
}

class CallFrameSQLFreeConnect : public CallFrame
{
public:
	struct Args
	{
		fXX(ConnectionHandle,		SQLHDBC)
	} args;
	CallFrameSQLFreeConnect(const Args &rArgs)
	: CallFrame(SQL_API_SQLFREEENV, "SQLFreeConnect", SQL_HANDLE_DBC), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(ConnectionHandle,		SQLHDBC)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLFreeConnect(CallFrameSQLFreeConnect::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLFreeConnect ? stack->Push(new CallFrameSQLFreeConnect(args)) : -1;
}

class CallFrameSQLAllocStmt : public CallFrame
{
public:
	struct Args
	{
		fXX(ConnectionHandle,		SQLHDBC)
		fXX(StatementHandle,		SQLHSTMT*)
	} args;
	CallFrameSQLAllocStmt(const Args &rArgs)
	: CallFrame(SQL_API_SQLALLOCSTMT, "SQLAllocStmt", SQL_HANDLE_DBC), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(ConnectionHandle,		SQLHDBC)
			fPX(StatementHandle,		SQLHSTMT*)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLAllocStmt(CallFrameSQLAllocStmt::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLAllocStmt ? stack->Push(new CallFrameSQLAllocStmt(args)) : -1;
}

class CallFrameSQLFreeStmt : public CallFrame
{
public:
	struct Args
	{
		fXX(StatementHandle,		SQLHSTMT)
		fXX(Option,					SQLUSMALLINT)
	} args;
	Legend Option;
	CallFrameSQLFreeStmt(const Args &rArgs)
	: CallFrame(SQL_API_SQLFREESTMT, "SQLFreeStmt", SQL_HANDLE_STMT), args(rArgs)
	, Option(args.Option, Legend::SqlFreeStmtOption)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(StatementHandle,		SQLHSTMT)
			fHA(Option,					SQLUSMALLINT)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLFreeStmt(CallFrameSQLFreeStmt::Args args)
{
	if (args.Option == SQL_DROP)
	{
		::EnterCriticalSection(&CallStack::cs);
		if (StatementInfo *pInfo = StatementInfo::FromHandle(args.StatementHandle))
		{
			StatementInfo::Delete(pInfo);
			CoTaskMemFree(pInfo->StatementText);
			CoTaskMemFree(pInfo);
		}
		::LeaveCriticalSection(&CallStack::cs);
	}
	return stack->HideLevel() < ODBCTrace.SQLFreeStmt ? stack->Push(new CallFrameSQLFreeStmt(args)) : -1;
}

class CallFrameSQLAllocHandle : public CallFrame
{
public:
	struct Args
	{
		fXX(HandleType,				SQLSMALLINT)
		fXX(InputHandle,			SQLHANDLE)
		fXX(OutputHandlePtr,		SQLHANDLE*)
	} args;
	CallFrameSQLAllocHandle(const Args &rArgs)
	: CallFrame(SQL_API_SQLALLOCHANDLE, "SQLAllocHandle"), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fHX(HandleType,				SQLSMALLINT)
			fGX(InputHandle,			SQLHANDLE)
			fPX(OutputHandlePtr,		SQLHANDLE*)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLAllocHandle(CallFrameSQLAllocHandle::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLAllocHandle ? stack->Push(new CallFrameSQLAllocHandle(args)) : -1;
}

class CallFrameSQLFreeHandle : public CallFrame
{
public:
	struct Args
	{
		fXX(HandleType,				SQLSMALLINT)
		fXX(Handle,					SQLHANDLE)
	} args;
	CallFrameSQLFreeHandle(const Args &rArgs)
	: CallFrame(SQL_API_SQLFREEHANDLE, "SQLFreeHandle"), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fHX(HandleType,				SQLSMALLINT)
			fGX(Handle,					SQLHANDLE)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLFreeHandle(CallFrameSQLFreeHandle::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLFreeHandle ? stack->Push(new CallFrameSQLFreeHandle(args)) : -1;
}

class CallFrameSQLSetCursorName : public CallFrame
{
public:
	struct Args
	{
		fXX(StatementHandle,		SQLHSTMT)
		fXX(CursorName,				SQLCHAR*)
		fXX(NameLength,				SQLSMALLINT)
	} args;
	CallFrameSQLSetCursorName(const Args &rArgs)
	: CallFrame(SQL_API_SQLSETCURSORNAME, "SQLSetCursorName", SQL_HANDLE_STMT), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(StatementHandle,		SQLHSTMT)
			fHS(CursorName,				SQLCHAR*)
			fHX(NameLength,				SQLSMALLINT)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLSetCursorName(CallFrameSQLSetCursorName::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLSetCursorName ? stack->Push(new CallFrameSQLSetCursorName(args)) : -1;
}

class CallFrameSQLSetCursorNameW : public CallFrame
{
public:
	struct Args
	{
		fXX(StatementHandle,		SQLHSTMT)
		fXX(CursorName,				SQLWCHAR*)
		fXX(NameLength,				SQLSMALLINT)
	} args;
	CallFrameSQLSetCursorNameW(const Args &rArgs)
	: CallFrame(SQL_API_SQLSETCURSORNAME, "SQLSetCursorNameW", SQL_HANDLE_STMT), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(StatementHandle,		SQLHSTMT)
			fLS(CursorName,				SQLWCHAR*)
			fHX(NameLength,				SQLSMALLINT)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLSetCursorNameW(CallFrameSQLSetCursorNameW::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLSetCursorName ? stack->Push(new CallFrameSQLSetCursorNameW(args)) : -1;
}

class CallFrameSQLGetCursorName : public CallFrame
{
public:
	struct Args
	{
		fXX(StatementHandle,		SQLHSTMT)
		fXX(CursorName,				SQLCHAR*)
		fXX(BufferLength,			SQLSMALLINT)
		fXX(NameLengthPtr,			SQLSMALLINT*)
	} args;
	CallFrameSQLGetCursorName(const Args &rArgs)
	: CallFrame(SQL_API_SQLGETCURSORNAME, "SQLGetCursorName", SQL_HANDLE_STMT), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(StatementHandle,		SQLHSTMT)
			fPX(CursorName,				SQLCHAR*)
			fHX(BufferLength,			SQLSMALLINT)
			fPX(NameLengthPtr,			SQLSMALLINT*)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLGetCursorName(CallFrameSQLGetCursorName::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLGetCursorName ? stack->Push(new CallFrameSQLGetCursorName(args)) : -1;
}

class CallFrameSQLGetCursorNameW : public CallFrame
{
public:
	struct Args
	{
		fXX(StatementHandle,		SQLHSTMT)
		fXX(CursorName,				SQLWCHAR*)
		fXX(BufferLength,			SQLSMALLINT)
		fXX(NameLengthPtr,			SQLSMALLINT*)
	} args;
	CallFrameSQLGetCursorNameW(const Args &rArgs)
	: CallFrame(SQL_API_SQLGETCURSORNAME, "SQLGetCursorNameW", SQL_HANDLE_STMT), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(StatementHandle,		SQLHSTMT)
			fPX(CursorName,				SQLWCHAR*)
			fHX(BufferLength,			SQLSMALLINT)
			fPX(NameLengthPtr,			SQLSMALLINT*)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLGetCursorNameW(CallFrameSQLGetCursorNameW::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLGetCursorName ? stack->Push(new CallFrameSQLGetCursorNameW(args)) : -1;
}

class CallFrameSQLSetPos : public CallFrame
{
public:
	struct Args
	{
		fXX(StatementHandle,		SQLHSTMT)
		fXX(RowNumber,				SQLUSMALLINT)
		fXX(Operation,				SQLUSMALLINT)
		fXX(LockType,				SQLUSMALLINT)
	} args;
	Legend Operation, LockType;
	CallFrameSQLSetPos(const Args &rArgs)
	: CallFrame(SQL_API_SQLSETPOS, "SQLSetPos", SQL_HANDLE_STMT), args(rArgs)
	, Operation(args.Operation, Legend::SqlSetPosOperation)
	, LockType(args.LockType, Legend::SqlSetPosLockType)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(StatementHandle,		SQLHSTMT)
			fHX(RowNumber,				SQLUSMALLINT)
			fHA(Operation,				SQLUSMALLINT)
			fHA(LockType,				SQLUSMALLINT)
		);
		CallFrame::Dump(fmt);
	}
	virtual char *DumpDiagFields(RETCODE)
	{
		return StatementInfo::ColumnDump(args.StatementHandle);
	}
};

RETCODE SQL_API TraceSQLSetPos(CallFrameSQLSetPos::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLSetPos ? stack->Push(new CallFrameSQLSetPos(args)) : -1;
}

class CallFrameSQLBulkOperations : public CallFrame
{
public:
	struct Args
	{
		fXX(StatementHandle,		SQLHSTMT)
		fXX(Operation,				SQLUSMALLINT)
	} args;
	CallFrameSQLBulkOperations(const Args &rArgs)
	: CallFrame(SQL_API_SQLBULKOPERATIONS, "SQLBulkOperations", SQL_HANDLE_STMT), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(StatementHandle,		SQLHSTMT)
			fHX(Operation,				SQLUSMALLINT)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLBulkOperations(CallFrameSQLBulkOperations::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLBulkOperations ? stack->Push(new CallFrameSQLBulkOperations(args)) : -1;
}

class CallFrameSQLCloseCursor : public CallFrame
{
public:
	struct Args
	{
		fXX(StatementHandle,		SQLHSTMT)
	} args;
	CallFrameSQLCloseCursor(const Args &rArgs)
	: CallFrame(SQL_API_SQLCLOSECURSOR, "SQLCloseCursor", SQL_HANDLE_STMT), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(StatementHandle,		SQLHSTMT)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLCloseCursor(CallFrameSQLCloseCursor::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLCloseCursor ? stack->Push(new CallFrameSQLCloseCursor(args)) : -1;
}

class CallFrameSQLGetInfo : public CallFrame
{
public:
	struct Args
	{
		fXX(ConnectionHandle,		SQLHDBC)
		fXX(InfoType,				SQLUSMALLINT)
		fXX(InfoValuePtr,			SQLPOINTER)
		fXX(BufferLength,			SQLUSMALLINT)
		fXX(StringLengthPtr,		SQLUSMALLINT*)
	} args;
	CallFrameSQLGetInfo(const Args &rArgs)
	: CallFrame(SQL_API_SQLGETINFO, "SQLGetInfo", SQL_HANDLE_DBC), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(ConnectionHandle,		SQLHDBC)
			fHX(InfoType,				SQLUSMALLINT)
			fPX(InfoValuePtr,			SQLPOINTER)
			fHX(BufferLength,			SQLUSMALLINT)
			fPX(StringLengthPtr,		SQLUSMALLINT*)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLGetInfo(CallFrameSQLGetInfo::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLGetInfo ? stack->Push(new CallFrameSQLGetInfo(args)) : -1;
}

class CallFrameSQLGetInfoW : public CallFrame
{
public:
	struct Args
	{
		fXX(ConnectionHandle,		SQLHDBC)
		fXX(InfoType,				SQLUSMALLINT)
		fXX(InfoValuePtr,			SQLPOINTER)
		fXX(BufferLength,			SQLUSMALLINT)
		fXX(StringLengthPtr,		SQLUSMALLINT*)
	} args;
	CallFrameSQLGetInfoW(const Args &rArgs)
	: CallFrame(SQL_API_SQLGETINFO, "SQLGetInfoW", SQL_HANDLE_DBC), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(ConnectionHandle,		SQLHDBC)
			fHX(InfoType,				SQLUSMALLINT)
			fPX(InfoValuePtr,			SQLPOINTER)
			fHX(BufferLength,			SQLUSMALLINT)
			fPX(StringLengthPtr,		SQLUSMALLINT*)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLGetInfoW(CallFrameSQLGetInfoW::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLGetInfo ? stack->Push(new CallFrameSQLGetInfoW(args)) : -1;
}

class CallFrameSQLGetTypeInfo : public CallFrame
{
public:
	struct Args
	{
		fXX(StatementHandle,		SQLHSTMT)
		fXX(DataType,				SQLSMALLINT)
	} args;
	CallFrameSQLGetTypeInfo(const Args &rArgs)
	: CallFrame(SQL_API_SQLGETTYPEINFO, "SQLGetTypeInfo", SQL_HANDLE_STMT), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(StatementHandle,		SQLHSTMT)
			fHX(DataType,				SQLSMALLINT)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLGetTypeInfo(CallFrameSQLGetTypeInfo::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLGetTypeInfo ? stack->Push(new CallFrameSQLGetTypeInfo(args)) : -1;
}

class CallFrameSQLGetTypeInfoW : public CallFrame
{
public:
	struct Args
	{
		fXX(StatementHandle,		SQLHSTMT)
		fXX(DataType,				SQLSMALLINT)
	} args;
	CallFrameSQLGetTypeInfoW(const Args &rArgs)
	: CallFrame(SQL_API_SQLGETTYPEINFO, "SQLGetTypeInfoW", SQL_HANDLE_STMT), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(StatementHandle,		SQLHSTMT)
			fHX(DataType,				SQLSMALLINT)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLGetTypeInfoW(CallFrameSQLGetTypeInfoW::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLGetTypeInfo ? stack->Push(new CallFrameSQLGetTypeInfoW(args)) : -1;
}

class CallFrameSQLGetFunctions : public CallFrame
{
public:
	struct Args
	{
		fXX(ConnectionHandle,		SQLHDBC)
		fXX(FunctionId,				SQLUSMALLINT)
		fXX(SupportedPtr,			SQLUSMALLINT*)
	} args;
	CallFrameSQLGetFunctions(const Args &rArgs)
	: CallFrame(SQL_API_SQLGETFUNCTIONS, "SQLGetFunctions", SQL_HANDLE_DBC), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(ConnectionHandle,		SQLHDBC)
			fHX(FunctionId,				SQLUSMALLINT)
			fPX(SupportedPtr,			SQLUSMALLINT*)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLGetFunctions(CallFrameSQLGetFunctions::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLGetFunctions ? stack->Push(new CallFrameSQLGetFunctions(args)) : -1;
}

class CallFrameSQLGetDiagField : public CallFrame
{
public:
	struct Args
	{
		fXX(HandleType,				SQLSMALLINT)
		fXX(Handle,					SQLHANDLE)
		fXX(RecNumber,				SQLSMALLINT)
		fXX(DiagIdentifier,			SQLSMALLINT)
		fXX(DiagInfoPtr,			SQLPOINTER)
		fXX(BufferLength,			SQLSMALLINT)
		fXX(StringLengthPtr,		SQLSMALLINT*)
	} args;
	CallFrameSQLGetDiagField(const Args &rArgs)
	: CallFrame(SQL_API_SQLGETDIAGFIELD, "SQLGetDiagField"), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fHX(HandleType,				SQLSMALLINT)
			fGX(Handle,					SQLHANDLE)
			fHX(RecNumber,				SQLSMALLINT)
			fHX(DiagIdentifier,			SQLSMALLINT)
			fPX(DiagInfoPtr,			SQLPOINTER)
			fHX(BufferLength,			SQLSMALLINT)
			fPX(StringLengthPtr,		SQLSMALLINT*)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLGetDiagField(CallFrameSQLGetDiagField::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLGetDiagField ? stack->Push(new CallFrameSQLGetDiagField(args)) : -1;
}

class CallFrameSQLGetDiagFieldW : public CallFrame
{
public:
	struct Args
	{
		fXX(HandleType,				SQLSMALLINT)
		fXX(Handle,					SQLHANDLE)
		fXX(RecNumber,				SQLSMALLINT)
		fXX(DiagIdentifier,			SQLSMALLINT)
		fXX(DiagInfoPtr,			SQLPOINTER)
		fXX(BufferLength,			SQLSMALLINT)
		fXX(StringLengthPtr,		SQLSMALLINT*)
	} args;
	CallFrameSQLGetDiagFieldW(const Args &rArgs)
	: CallFrame(SQL_API_SQLGETDIAGFIELD, "SQLGetDiagFieldW"), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fHX(HandleType,				SQLSMALLINT)
			fGX(Handle,					SQLHANDLE)
			fHX(RecNumber,				SQLSMALLINT)
			fHX(DiagIdentifier,			SQLSMALLINT)
			fPX(DiagInfoPtr,			SQLPOINTER)
			fHX(BufferLength,			SQLSMALLINT)
			fPX(StringLengthPtr,		SQLSMALLINT*)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLGetDiagFieldW(CallFrameSQLGetDiagFieldW::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLGetDiagField ? stack->Push(new CallFrameSQLGetDiagFieldW(args)) : -1;
}

class CallFrameSQLGetDiagRec : public CallFrame
{
public:
	struct Args
	{
		fXX(HandleType,				SQLSMALLINT)
		fXX(Handle,					SQLHANDLE)
		fXX(RecNumber,				SQLSMALLINT)
		fXX(Sqlstate,				SQLCHAR*)
		fXX(NativeErrorPtr,			SQLINTEGER*)
		fXX(MessageText,			SQLCHAR*)
		fXX(BufferLength,			SQLSMALLINT)
		fXX(TextLengthPtr,			SQLSMALLINT*)
	} args;
	CallFrameSQLGetDiagRec(const Args &rArgs)
	: CallFrame(SQL_API_SQLGETDIAGREC, "SQLGetDiagRec"), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fHX(HandleType,				SQLSMALLINT)
			fGX(Handle,					SQLHANDLE)
			fHX(RecNumber,				SQLSMALLINT)
			fPX(Sqlstate,				SQLCHAR*)
			fPX(NativeErrorPtr,			SQLINTEGER*)
			fPX(MessageText,			SQLCHAR*)
			fHX(BufferLength,			SQLSMALLINT)
			fPX(TextLengthPtr,			SQLSMALLINT*)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLGetDiagRec(CallFrameSQLGetDiagRec::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLGetDiagRec ? stack->Push(new CallFrameSQLGetDiagRec(args)) : -1;
}

class CallFrameSQLGetDiagRecW : public CallFrame
{
public:
	struct Args
	{
		fXX(HandleType,				SQLSMALLINT)
		fXX(Handle,					SQLHANDLE)
		fXX(RecNumber,				SQLSMALLINT)
		fXX(Sqlstate,				SQLWCHAR*)
		fXX(NativeErrorPtr,			SQLINTEGER*)
		fXX(MessageText,			SQLWCHAR*)
		fXX(BufferLength,			SQLSMALLINT)
		fXX(TextLengthPtr,			SQLSMALLINT*)
	} args;
	CallFrameSQLGetDiagRecW(const Args &rArgs)
	: CallFrame(SQL_API_SQLGETDIAGREC, "SQLGetDiagRecW"), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fHX(HandleType,				SQLSMALLINT)
			fGX(Handle,					SQLHANDLE)
			fHX(RecNumber,				SQLSMALLINT)
			fPX(Sqlstate,				SQLWCHAR*)
			fPX(NativeErrorPtr,			SQLINTEGER*)
			fPX(MessageText,			SQLWCHAR*)
			fHX(BufferLength,			SQLSMALLINT)
			fPX(TextLengthPtr,			SQLSMALLINT*)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLGetDiagRecW(CallFrameSQLGetDiagRecW::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLGetDiagRec ? stack->Push(new CallFrameSQLGetDiagRecW(args)) : -1;
}

class CallFrameSQLError : public CallFrame
{
public:
	struct Args
	{
		fXX(EnvironmentHandle,		SQLHENV)
		fXX(ConnectionHandle,		SQLHDBC)
		fXX(StatementHandle,		SQLHSTMT)
		fXX(Sqlstate,				SQLCHAR*)
		fXX(NativeErrorPtr,			SQLINTEGER*)
		fXX(MessageText,			SQLCHAR*)
		fXX(BufferLength,			SQLSMALLINT)
		fXX(TextLengthPtr,			SQLSMALLINT*)
	} args;
	CallFrameSQLError(const Args &rArgs)
	: CallFrame(SQL_API_SQLERROR, "SQLError"), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(EnvironmentHandle,		SQLHENV)
			fGX(ConnectionHandle,		SQLHDBC)
			fGX(StatementHandle,		SQLHSTMT)
			fPX(Sqlstate,				SQLCHAR*)
			fPX(NativeErrorPtr,			SQLINTEGER*)
			fPX(MessageText,			SQLCHAR*)
			fHX(BufferLength,			SQLSMALLINT)
			fPX(TextLengthPtr,			SQLSMALLINT*)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLError(CallFrameSQLError::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLError ? stack->Push(new CallFrameSQLError(args)) : -1;
}

class CallFrameSQLErrorW : public CallFrame
{
public:
	struct Args
	{
		fXX(EnvironmentHandle,		SQLHENV)
		fXX(ConnectionHandle,		SQLHDBC)
		fXX(StatementHandle,		SQLHSTMT)
		fXX(Sqlstate,				SQLWCHAR*)
		fXX(NativeErrorPtr,			SQLINTEGER*)
		fXX(MessageText,			SQLWCHAR*)
		fXX(BufferLength,			SQLSMALLINT)
		fXX(TextLengthPtr,			SQLSMALLINT*)
	} args;
	CallFrameSQLErrorW(const Args &rArgs)
	: CallFrame(SQL_API_SQLERROR, "SQLErrorW"), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(EnvironmentHandle,		SQLHENV)
			fGX(ConnectionHandle,		SQLHDBC)
			fGX(StatementHandle,		SQLHSTMT)
			fPX(Sqlstate,				SQLWCHAR*)
			fPX(NativeErrorPtr,			SQLINTEGER*)
			fPX(MessageText,			SQLWCHAR*)
			fHX(BufferLength,			SQLSMALLINT)
			fPX(TextLengthPtr,			SQLSMALLINT*)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLErrorW(CallFrameSQLErrorW::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLError ? stack->Push(new CallFrameSQLErrorW(args)) : -1;
}

class CallFrameSQLCopyDesc : public CallFrame
{
public:
	struct Args
	{
		fXX(SoureDescHandle,		SQLHDESC)
		fXX(TargetDescHandle,		SQLHDESC)
	} args;
	CallFrameSQLCopyDesc(const Args &rArgs)
	: CallFrame(SQL_API_SQLCOPYDESC, "SQLCopyDesc"), args(rArgs)
	{
	}
	virtual void Dump()
	{
		static const char fmt[]
		(
			fGX(SoureDescHandle,		SQLHDESC)
			fGX(TargetDescHandle,		SQLHDESC)
		);
		CallFrame::Dump(fmt);
	}
};

RETCODE SQL_API TraceSQLCopyDesc(CallFrameSQLCopyDesc::Args args)
{
	return stack->HideLevel() < ODBCTrace.SQLCopyDesc ? stack->Push(new CallFrameSQLCopyDesc(args)) : -1;
}
