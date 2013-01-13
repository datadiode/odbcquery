/*/DatabaseEx.cpp

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
#include <afxpriv.h>
#include "DatabaseEx.h"
#include "../Common/VersionData.h"
#include "midl/MSRDO20_i.c"

static LPFNGETCLASSOBJECT rdoDllGetClassObject = 0;

static STDMETHODIMP rdoCreateDispatch(REFCLSID clsid, REFIID riid, void **ppdisp)
{
	IClassFactory2 *pcf = 0;
	HRESULT hr = rdoDllGetClassObject(clsid, IID_IClassFactory2, (void **)&pcf);
	if (SUCCEEDED(hr))
	{
		static const WCHAR rgkey[] =
#			include "midl/MSRDO20.lic"
		BSTR key = SysAllocStringLen(rgkey, sizeof(rgkey)/sizeof(*rgkey));
		hr = pcf->CreateInstanceLic(0, 0, riid, key, ppdisp);
		SysFreeString(key);
		pcf->Release();
	}
	return hr;
}

_rdoEngine *CDatabaseEx::m_rdoEngine = 0;
rdoEnvironments *CDatabaseEx::m_rdoEnvironments = 0;
_rdoEnvironment *CDatabaseEx::m_rdoEnvironment = 0;

void CDatabaseEx::Uninitialize()
{
	if (void *p = InterlockedExchangePointer(reinterpret_cast<void **>(&m_rdoEngine), 0))
		reinterpret_cast<IUnknown *>(p)->Release();
	if (void *p = InterlockedExchangePointer(reinterpret_cast<void **>(&m_rdoEnvironments), 0))
		reinterpret_cast<IUnknown *>(p)->Release();
	if (void *p = InterlockedExchangePointer(reinterpret_cast<void **>(&m_rdoEnvironment), 0))
		reinterpret_cast<IUnknown *>(p)->Release();
}

#ifdef _DEBUG
long RefCount(LPUNKNOWN punk)
{
	long cRef = 0;
	if (punk)
	{
		cRef = punk->AddRef();
		cRef = punk->Release();
	}
	TRACE("RefCount = %ld\n", cRef);
	return cRef;
}
#endif

BOOL CDatabaseEx::OpenEx(LPCTSTR lpszConnectString, DWORD dwOptions)
{
	// If we lack RDO, connect through MFC as usual.
	if (m_rdoEngine == 0)
		return CDatabase::OpenEx(lpszConnectString, dwOptions);

	CComBSTR bstrConnect;

	m_bUpdatable = !(dwOptions & openReadOnly);

	if (m_rdoConnxn == 0)
	{
		ASSERT(m_hdbc == 0);
		AfxCheckError(rdoCreateDispatch(CLSID_rdoConnection, IID__rdoConnection, (void **)&m_rdoConnxn));
		ASSERT(RefCount(m_rdoConnxn));
		m_rdoConnxn->get_hDbc(reinterpret_cast<long *>(&m_hdbc));
		ASSERT(RefCount(m_rdoConnxn));
		m_rdoConnxn->get_LoginTimeout(reinterpret_cast<long *>(&m_dwLoginTimeout));
		m_rdoConnxn->get_QueryTimeout(reinterpret_cast<long *>(&m_dwQueryTimeout));
		ASSERT(RefCount(m_rdoConnxn));
		// Turn on cursor lib support
		if (dwOptions & CDatabase::useCursorLib)
		{
			m_rdoConnxn->put_CursorDriver(rdUseOdbc);
			// With cursor library added records immediately in result set
			m_bIncRecordCountOnAdd = TRUE;
		}
		m_strConnect = lpszConnectString;
		bstrConnect.Attach(m_strConnect.AllocSysString());
		m_rdoConnxn->put_Connect(bstrConnect);
	}

	ASSERT(m_hdbc);
	ASSERT_VALID(this);
	ASSERT(lpszConnectString == NULL || AfxIsValidString(lpszConnectString));
	ASSERT(!(dwOptions & noOdbcDialog && dwOptions & forceOdbcDialog));

	// Exclusive access not supported.
	ASSERT(!(dwOptions & openExclusive));

	long nPrompt = rdDriverComplete;

	if (dwOptions & noOdbcDialog)
		nPrompt = rdDriverNoPrompt;
	else if (dwOptions & forceOdbcDialog)
		nPrompt = rdDriverPrompt;

	ASSERT(RefCount(m_rdoConnxn));
	m_rdoConnxn->EstablishConnection(
		CComVariant(nPrompt), CComVariant(!m_bUpdatable), CComVariant(0L));
	ASSERT(RefCount(m_rdoConnxn));
	m_rdoConnxn->get_Connect(&bstrConnect);
	m_strConnect = bstrConnect;
	ASSERT(RefCount(m_rdoConnxn));
	m_strConnect.Insert(0, _T("ODBC;"));
	// Verify support for required functionality and cache info
	VerifyConnect();
	GetConnectInfo();
	m_chIDQuoteChar = _T('"');
	return TRUE;
}

void CDatabaseEx::Close()
{
	if (m_rdoConnxn)
		m_hdbc = SQL_NULL_HDBC;
	// HSTMTs don't survive the ReleaseDispatch(), so CDatabase::Close() first!
	CDatabase::Close();
	if (void *p = InterlockedExchangePointer(reinterpret_cast<void **>(&m_rdoConnxn), 0))
		reinterpret_cast<IUnknown *>(p)->Release();
}

HSTMT CDatabaseEx::Prepare(LPCTSTR lpszSQL)
{
	HSTMT hstmt = 0;
	if (!Check(::SQLAllocStmt(m_hdbc, &hstmt)))
		ThrowDBException(SQL_INVALID_HANDLE);
	LPSTR lpszWSQL = AfxAllocTaskString(lpszSQL);
	ReplaceBrackets(lpszWSQL);
	RETCODE nRetCode = ::SQLPrepare(hstmt, (UCHAR*)lpszWSQL, SQL_NTS);
	CoTaskMemFree(lpszWSQL);
	return hstmt;
}

long CDatabaseEx::Finalize(HSTMT hstmt, RETCODE nRetCode)
{
	SQLINTEGER nRowsAffected = -1;
	CDBException *e = 0;
	if (!SQL_SUCCEEDED(nRetCode))
		(e = new CDBException(nRetCode))->BuildErrorString(this, hstmt);
	else if (!SQL_SUCCEEDED(::SQLRowCount(hstmt, &nRowsAffected)))
		nRowsAffected = -1;
	::SQLFreeStmt(hstmt, SQL_DROP);
	if (e)
		throw e;
	return nRowsAffected;
}

long CDatabaseEx::Execute(HSTMT hstmt)
{
	RETCODE nRetCode = ::SQLExecute(hstmt);
	return Finalize(hstmt, nRetCode);
}

long CDatabaseEx::Execute(LPCTSTR lpszSQL)
{
	HSTMT hstmt = 0;
	if (!Check(::SQLAllocStmt(m_hdbc, &hstmt)))
		ThrowDBException(SQL_INVALID_HANDLE);
	LPSTR lpszWSQL = AfxAllocTaskString(lpszSQL);
	ReplaceBrackets(lpszWSQL);
	RETCODE nRetCode = ::SQLExecDirect(hstmt, (UCHAR*)lpszWSQL, SQL_NTS);
	CoTaskMemFree(lpszWSQL);
	return Finalize(hstmt, nRetCode);
}

bool CDatabaseEx::PoweredBy(const char *whom) const
{
	char buffer[100];
	SWORD cbData;
	RETCODE rc = ::SQLGetInfo(m_hdbc, SQL_DBMS_NAME, buffer, 100, &cbData);
	return _memicmp(buffer, whom, strlen(whom)) == 0;
}

bool CDatabaseEx::ExtractTableName(CString &sName, LPCTSTR pszQualifiedName)
{
	return AfxExtractSubString(sName, pszQualifiedName, 3, m_chIDQuoteChar)
		|| AfxExtractSubString(sName, pszQualifiedName, 1, m_chIDQuoteChar);
}

bool CDatabaseEx::ExtractOwnerName(CString &sName, LPCTSTR pszQualifiedName)
{
	return AfxExtractSubString(sName, pszQualifiedName, 3, m_chIDQuoteChar)
		&& AfxExtractSubString(sName, pszQualifiedName, 1, m_chIDQuoteChar);
}

CDatabaseEx::CDatabaseEx()
: m_rdoConnxn(0)
{
}

CDatabaseEx::~CDatabaseEx()
{
	Close();
}

#if _MFC_VER >= 0x0700
  #include <../src/mfc/afximpl.h>
  #include <../src/mfc/dbimpl.h>
#else
  #include <../src/afximpl.h>
  #include <../src/dbimpl.h>
#endif

PROCESS_LOCAL(_AFX_DB_STATE, _afxDbState)

void CDatabaseEx::Initialize()
{
	if (HMODULE hDLL = ::CoLoadLibrary(L"MSRDO20.DLL", TRUE))
	{
		rdoDllGetClassObject = reinterpret_cast
			<LPFNGETCLASSOBJECT>(GetProcAddress(hDLL, "DllGetClassObject"));
		if (rdoDllGetClassObject)
		{
			AfxCheckError(rdoCreateDispatch(
				CLSID_rdoEngine, IID__rdoEngine, (void **)&m_rdoEngine));
			m_rdoEngine->get_rdoEnvironments(&m_rdoEnvironments);
			m_rdoEnvironments->get_Item(CComVariant(0L), &m_rdoEnvironment);
			// Pass the ODBC environment handle to MFC
			_AFX_DB_STATE* pDbState = _afxDbState;
			AfxLockGlobals(CRIT_ODBC);
			ASSERT(pDbState->m_henvAllConnections == SQL_NULL_HENV);
			ASSERT(pDbState->m_nAllocatedConnections == 0);
			m_rdoEnvironment->get_hEnv(
				reinterpret_cast<long *>(&pDbState->m_henvAllConnections));
			// Prevent CDatabase::Free() from killing our HENV
			pDbState->m_nAllocatedConnections = 1;
			AfxUnlockGlobals(CRIT_ODBC);
		}
	}
}
