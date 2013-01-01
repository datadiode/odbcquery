class CCryptSession
{
	HCRYPTPROV m_hProv;
	HCRYPTHASH m_hHash;
	HCRYPTKEY m_hKey;
public:
	CCryptSession(const BYTE *, DWORD, int fVolatile = 0);
	~CCryptSession();
	DWORD Encrypt(BYTE *, DWORD);
	DWORD Decrypt(BYTE *, DWORD);
};

