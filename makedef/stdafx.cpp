#include <windows.h>
#include <shlwapi.h>
#include <assert.h>
#define ASSERT assert

// Cope with VS2008's disregarding of #pragma intrinsic(memset)
#undef ZeroMemory
#define ZeroMemory SecureZeroMemory

#pragma intrinsic(memcmp)
