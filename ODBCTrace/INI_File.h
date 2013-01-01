class CINI_File
{
public:
	TCHAR szFileName[MAX_PATH];
	typedef const CINI_File *From;
	UINT GetString(LPCTSTR szSection, LPCTSTR szEntry, LPTSTR szBuffer, UINT cchBuffer) const;
	BOOL SetString(LPCTSTR szSection, LPCTSTR szEntry, LPCTSTR szBuffer) const;
	BOOL GetStruct(LPCTSTR szSection, LPCTSTR szEntry, LPVOID pvStruct, UINT cbStruct) const;
	BOOL SetStruct(LPCTSTR szSection, LPCTSTR szEntry, LPVOID pvStruct, UINT cbStruct) const;
	UINT GetNumber(LPCTSTR szSection, LPCTSTR szEntry, INT nDefault) const;
	BOOL SetNumber(LPCTSTR szSection, LPCTSTR szEntry, UINT nValue) const;
	COLORREF GetColor(LPCTSTR szSection, LPCTSTR szEntry, COLORREF crDefault) const;
	BOOL SetColor(LPCTSTR szSection, LPCTSTR szEntry, COLORREF crValue) const;
	void ScanWindowPlacement(HWND, LPCTSTR szSection, LPCTSTR szEntry) const;
	void DumpWindowPlacement(HWND, LPCTSTR szSection, LPCTSTR szEntry) const;
	void ScanHeaderItemSizes(HWND, LPCTSTR szSection, LPCTSTR szEntry) const;
	void DumpHeaderItemSizes(HWND, LPCTSTR szSection, LPCTSTR szEntry) const;
};
