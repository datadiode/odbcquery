struct DllProxy
{
	//struct VersionInfo {} version;
	LPCSTR Names[1];
	LPVOID Load();
	LPVOID EnsureLoad();
	template<class T> struct Instance;
};

template<class T> struct DllProxy::Instance
{
	union
	{
		struct
		{
			//DllProxy::VersionInfo version;
			LPCSTR Names[sizeof(T) / sizeof(LPCSTR)];
		} Names;
		DllProxy Proxy;
	};
	HMODULE H;
	operator T *()
	{
		return (T *)Proxy.Load();
	}
	T *operator->()
	{
		return (T *)Proxy.EnsureLoad();
	}
};

/**
 * @brief PSAPI dll proxy
 */
struct PSAPI
{
	BOOL(NTAPI*EnumProcesses)
	(
		DWORD *lpidProcess,	// array to receive the process identifiers
		DWORD cb,			// size of the array
		DWORD *cbNeeded		// receives the number of bytes returned
	);
	BOOL(NTAPI*EnumProcessModules)
	(
		HANDLE hProcess,	// handle to the process
		HMODULE *lphModule,	// array to receive the module handles
		DWORD cb,			// size of the array
		LPDWORD lpcbNeeded	// receives the number of bytes returned
	);
	DWORD(NTAPI*GetModuleFileNameEx)
	(
		HANDLE hProcess,
		HMODULE hModule,
		LPTSTR lpFilename,
		DWORD nSize
	);

	HMODULE H;
};

extern DllProxy::Instance<struct PSAPI> PSAPI;

/**
 * @brief TOOLHELP dll proxy
 */

// need some #defines from <tlhelp32.h>
#define TH32CS_SNAPHEAPLIST 0x00000001
#define TH32CS_SNAPPROCESS  0x00000002
#define TH32CS_SNAPTHREAD   0x00000004
#define TH32CS_SNAPMODULE   0x00000008
#define TH32CS_SNAPALL      (TH32CS_SNAPHEAPLIST | TH32CS_SNAPPROCESS | TH32CS_SNAPTHREAD | TH32CS_SNAPMODULE)
#define TH32CS_INHERIT      0x80000000
#define MAX_MODULE_NAME32 255

struct TOOLHELP
{
	struct MODULEENTRY32
	{
		DWORD   dwSize;
		DWORD   th32ModuleID;		// This module
		DWORD   th32ProcessID;		// owning process
		DWORD   GlblcntUsage;		// Global usage count on the module
		DWORD   ProccntUsage;		// Module usage count in th32ProcessID's context
		BYTE  * modBaseAddr;		// Base address of module in th32ProcessID's context
		DWORD   modBaseSize;		// Size in bytes of module starting at modBaseAddr
		HMODULE hModule;			// The hModule of this module in th32ProcessID's context
		char    szModule[MAX_MODULE_NAME32 + 1];
		char    szExePath[MAX_PATH];
	};

	HANDLE(WINAPI*CreateToolhelp32Snapshot)(DWORD dwFlags, DWORD th32ProcessID);
	BOOL(WINAPI*Module32First)(HANDLE, MODULEENTRY32 *);
	BOOL(WINAPI*Module32Next)(HANDLE, MODULEENTRY32 *);

	HMODULE H;
	typedef TOOLHELP *DLL;
};

extern DllProxy::Instance<struct TOOLHELP> TOOLHELP;

/**
 * @brief SHELL32NT6 dll proxy
 */
struct SHELL32NT6
{
	HRESULT(STDAPICALLTYPE*SHGetKnownFolderPath)(REFKNOWNFOLDERID, DWORD, HANDLE, PWSTR *);
	HMODULE H;
	typedef SHELL32NT6 *DLL;
};

extern DllProxy::Instance<struct SHELL32NT6> SHELL32NT6;
