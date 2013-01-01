#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <shellapi.h>
#include <richedit.h>
#include <commdlg.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <sqlext.h>
#include <transact.h>
#include <oledb.h>
#include <oledberr.h>
#include <msdadc.h>

// Cope with VS2008's disregarding of #pragma intrinsic(memset)
#undef ZeroMemory
#define ZeroMemory SecureZeroMemory

#ifdef _DEBUG
extern void NTAPI AssertionFailed(const char *file, int line);
#define ASSERT(f) { if (!(f)) AssertionFailed(__FILE__, __LINE__); }
#else
#define ASSERT(f)
#endif
