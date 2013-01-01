#pragma once

#include "midl/MSRDO20_h.h"

class CDatabaseEx : public CDatabase
{
public:
	static _rdoEngine *m_rdoEngine;
	static rdoEnvironments *m_rdoEnvironments;
	static _rdoEnvironment *m_rdoEnvironment;
	static void Initialize();
	static void Uninitialize();
	_rdoConnection *m_rdoConnxn;
	virtual BOOL OpenEx(LPCTSTR lpszConnectString, DWORD dwOptions = 0);
	virtual void Close();
	HSTMT Prepare(LPCTSTR);
	long Finalize(HSTMT, RETCODE);
	long Execute(HSTMT);
	long Execute(LPCTSTR);
	bool PoweredBy(const char *) const;
	bool ExtractTableName(CString &, LPCTSTR);
	bool ExtractOwnerName(CString &, LPCTSTR);
	CDatabaseEx();
	~CDatabaseEx();
	using CDatabase::m_listRecordsets;
};
