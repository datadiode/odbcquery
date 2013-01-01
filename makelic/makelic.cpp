/*/makedef.cpp

Last edit: 2013-01-01 Jochen Neubeck

[The MIT license]

Copyright (c) 2013 Jochen Neubeck

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
#include <shlwapi.h>
#include <atlbase.h>
#include "../Common/WinStdIO.h"

static const char usage[] = "\n"
"Usage: %ls <progid|clsid> [<dll file path>]\n";

STDAPI GetRegisteredClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppv)
{
	return CoGetClassObject(rclsid, CLSCTX_INPROC_SERVER, NULL, riid, ppv);
}

int WriteLicKey(LPCWSTR cls, LPCWSTR dll)
{
	CLSID clsid;
	if (FAILED(CLSIDFromString(cls, &clsid)) &&
		FAILED(CLSIDFromProgID(cls, &clsid)))
	{
		WriteTo<STD_ERROR_HANDLE>("Failed to obtain CLSID for %ls\n", cls);
		return 1;
	}
	CComPtr<IClassFactory2> pcf;
	LPFNGETCLASSOBJECT GetClassObject = GetRegisteredClassObject;
	if (dll != NULL)
	{
		HMODULE const hdll = LoadLibraryW(dll);
		if (hdll == NULL)
		{
			WriteTo<STD_ERROR_HANDLE>("Failed to load %ls\n", dll);
			return 1;
		}
		GetClassObject = reinterpret_cast
			<LPFNGETCLASSOBJECT>(GetProcAddress(hdll, "DllGetClassObject"));
		if (GetClassObject == NULL)
		{
			WriteTo<STD_ERROR_HANDLE>("Failed to obtain DllGetClassObject\n");
			return 1;
		}
	}
	if (FAILED(GetClassObject(clsid, IID_IClassFactory2, (void **)&pcf)))
	{
		WriteTo<STD_ERROR_HANDLE>("Failed to obtain IClassFactory2\n");
		return 1;
	}
	CComBSTR key;
	if (FAILED(pcf->RequestLicKey(0, &key)))
	{
		WriteTo<STD_ERROR_HANDLE>("Failed to obtain license key\n");
		return 1;
	}
	int len = key.Length();
	char c = '{';
	for (int i = 0 ; i < len ; ++i)
	{
		if (i % 8 == 0)
			WriteTo<STD_OUTPUT_HANDLE>("\r\n");
		WriteTo<STD_OUTPUT_HANDLE>("%c 0x%04X", c, (UINT)(WORD)key[i]);
		c = ',';
	}
	WriteTo<STD_OUTPUT_HANDLE>(" };\r\n\r\n");
	return 0;
}

int mainCRTStartup()
{
	int argc = 0;
	LPWSTR args = GetCommandLineW();
	LPWSTR *argv = CommandLineToArgvW(args, &argc);
	args = PathGetArgsW(args);
	const LPCWSTR prog = PathFindFileNameW(argv[0]);
	if (argc < 2)
	{
		WriteTo<STD_ERROR_HANDLE>(usage, prog);
		return 1;
	}
	WriteTo<STD_OUTPUT_HANDLE>("/* %ls %ls */\r\n", prog, args);
	CoInitialize(NULL);
	int ret = WriteLicKey(argv[1], argc > 2 ? argv[2] : NULL);
	if (ret != 0)
	{
		WriteTo<STD_OUTPUT_HANDLE>("L\"NO LICENSE AVAILABLE\";\r\n");
	}
	CoUninitialize();
	LocalFree(argv);
	return ret;
}
