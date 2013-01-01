#include "stdafx.h"
#include "CryptSession.h"

CCryptSession::CCryptSession(const BYTE *rgPassword, DWORD dwLength, int fVolatile)
: m_hProv(0), m_hHash(0), m_hKey(0)
{
	if (CryptAcquireContext(&m_hProv, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT) &&
		CryptCreateHash(m_hProv, CALG_MD5, 0, 0, &m_hHash))
	{
		BYTE rgAtom[256];
		if (fVolatile)
		{
			DWORD dwBits = dwLength << 3;
			memcpy(rgAtom, rgPassword, dwLength);
			memset(rgAtom + dwLength, 'x', dwBits);
			rgAtom[dwLength + dwBits] = '\0';
			ATOM atom = GlobalFindAtomA((LPCSTR)rgAtom);
			if (atom)
			{
				fVolatile = 2;
			}
			else
			{
				CryptGenRandom(m_hProv, dwLength, rgAtom);
				while (dwBits)
				{
					--dwBits;
					if (rgAtom[dwBits >> 3] & 1 << (dwBits & 7))
						rgAtom[dwLength + dwBits] = 'X';
				}
				memcpy(rgAtom, rgPassword, dwLength);
				atom = GlobalAddAtomA((LPSTR)rgAtom);
				rgPassword = 0;
			}
			if (fVolatile == 2)
			{
				GlobalGetAtomName(atom, (LPSTR)rgAtom, sizeof rgAtom);
				memset(rgAtom, 0, dwLength);
				while (dwBits)
				{
					--dwBits;
					if (rgAtom[dwLength + dwBits] == 'X')
						rgAtom[dwBits >> 3] |= 1 << (dwBits & 7);
				}
				rgPassword = rgAtom;
			}
		}
		if (rgPassword && CryptHashData(m_hHash, rgPassword, dwLength, 0))
		{
			CryptDeriveKey(m_hProv, CALG_RC4, m_hHash, CRYPT_EXPORTABLE, &m_hKey);
		}
	}
}

CCryptSession::~CCryptSession()
{
	CryptDestroyKey(m_hKey);
	CryptDestroyHash(m_hHash);
	CryptReleaseContext(m_hProv, 0);
}

DWORD CCryptSession::Encrypt(BYTE *rgPassword, DWORD dwLength)
{
	DWORD dwResult = 0;
	if (CryptEncrypt(m_hKey, 0, TRUE, 0, rgPassword, &dwLength, dwLength))
	{
		dwResult = dwLength;
	}
	return dwResult;
}

DWORD CCryptSession::Decrypt(BYTE *rgPassword, DWORD dwLength)
{
	DWORD dwResult = 0;
	if (CryptDecrypt(m_hKey, 0, TRUE, 0, rgPassword, &dwLength))
	{
		dwResult = dwLength;
	}
	return dwResult;
}
