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
#include "stdafx.h"
#include <atlbase.h>
#include <dia2.h>
#include <cvconst.h>
#include "../Common/WinStdIO.h"
#include "../Common/TextReader.h"
#include "../Common/ModuleDirectory.h"

static const char usage[] = "\n"
"Usage: %ls <dll file path> [<pdb file path>] [<def file path>]\n"
"\n"
"This tool performs a dll vs. pdb match to find symbolic names for ordinal\n"
"exports. Results are written to stdout using def file format. Symbols from\n"
"an existing def file specified in the 3rd argument will be echoed without\n"
"change, but excluded from the dll vs. pdb match results. Of the remaining\n"
"matches, only those which do not conflict with a symbol or ordinal from the\n"
"existing def file will be included in the output.\n"
"\n"
"As an example, for upgrading the MFC42.DEF from Digital Mars you would use\n"
"arguments like follows:\n"
"\n"
"<dll file path> = \"%%SystemRoot%%\\System32\\mfc42.dll\"\n"
"<pdb file path> = \"%%SystemRoot%%\\symbols\\dll\\mfc42.pdb\"\n"
"<def file path> = \"C:\\dm\\MFC\\src\\intel\\MFC42.DEF\"\n";

static bool TriggersLNK4102(LPCSTR name)
{
	return name[0] == '?' && name[1] == '?' && name[2] == '_' &&
		(name[3] == 'E' || name[3] == 'G');
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
	WriteTo<STD_OUTPUT_HANDLE>("; %ls %ls\r\n\r\n", prog, args);
	const LPCWSTR dll = argv[1];
	ATOM atom = 0;
	WORD ordinal;
	bool *ordinal_used = static_cast<bool *>(
		LocalAlloc(LPTR, 65536 * sizeof *ordinal_used));
	if (argc > 3)
	{
		const LPCWSTR def = argv[3];
		CComPtr<IStream> pStream;
		if (SUCCEEDED(SHCreateStreamOnFileW(def, STGM_READ, &pStream)))
		{
			TextReader reader = pStream;
			BYTE eol = reader.allocCtype("\n");
			char *line = NULL;
			while (reader.readLine(&line, eol))
			{
				PathRemoveBlanksA(line);
				WriteTo<STD_OUTPUT_HANDLE>(line);
				int iVal = 0;
				LPSTR sep = StrStrA(line, " @ ");
				if (sep != NULL &&
					StrToIntExA(sep + 3, STIF_DEFAULT, &iVal) &&
					HIWORD(iVal) == 0)
				{
					ordinal = LOWORD(iVal);
					ordinal_used[ordinal] = true;
					PathRemoveArgsA(line);
					atom = AddAtomA(line);
				}
			}
			CoTaskMemFree(line);
		}
		else
		{
			WriteTo<STD_ERROR_HANDLE>("Failed to load %ls\n", def);
		}
		WriteTo<STD_OUTPUT_HANDLE>("\r\n; %ls generated content follows\r\n", prog);
	}
	else
	{
		WriteTo<STD_OUTPUT_HANDLE>("EXPORTS\r\n", PathFindFileNameW(dll));
	}
	CoInitialize(NULL);
	HMODULE hdll = LoadLibraryExW(dll, NULL, LOAD_LIBRARY_AS_DATAFILE);
	if (hdll == NULL)
	{
		WriteTo<STD_ERROR_HANDLE>("Failed to load %ls\n", dll);
	}
	else
	{
		CModuleDirectory md = hdll;
		CComPtr<IDiaEnumSymbolsByAddr> pSymbolsByAddr;
		if (argc > 2)
		{
			const LPCWSTR pdb = argv[2];
			CComPtr<IDiaDataSource> pIDiaDataSource;
			if (FAILED(CoCreateInstance(
				__uuidof(DiaSource), NULL, CLSCTX_INPROC_SERVER,
				__uuidof(IDiaDataSource), (void **)&pIDiaDataSource)))
			{
				WriteTo<STD_ERROR_HANDLE>("Failed to create DiaSource\n");
			}
			else if (FAILED(pIDiaDataSource->loadDataFromPdb(pdb)))
			{
				WriteTo<STD_ERROR_HANDLE>("Failed to load %ls\n", pdb);
			}
			else
			{
				CComPtr<IDiaSession> pSession;
				pIDiaDataSource->openSession(&pSession);
				pSession->getSymbolsByAddr(&pSymbolsByAddr);
			}
		}
		ULONG *rgAddressOfNames = (ULONG *)md.GetAddress(md.pEAT->AddressOfNames);
		ULONG *rgAddressOfFunctions = (ULONG *)md.GetAddress(md.pEAT->AddressOfFunctions);
		WORD *rgNameIndexes = (WORD *)md.GetAddress(md.pEAT->AddressOfNameOrdinals);
		DWORD i = 0;
		DWORD prevAddress = 0;
		while (i < md.pEAT->NumberOfFunctions)
		{
			WORD ordinal = static_cast<WORD>(md.pEAT->Base + i);
			if (ordinal_used[ordinal] == false)
			{
				if (DWORD address = rgAddressOfFunctions[i])
				{
					if (i < md.pEAT->NumberOfNames)
					{
						LPCSTR name = static_cast<LPCSTR>(md.GetAddress(rgAddressOfNames[i]));
						if (FindAtomA(name) == 0 && !TriggersLNK4102(name))
						{
							WriteTo<STD_OUTPUT_HANDLE>("%hs\r\n", name, ordinal);
							atom = AddAtomA(name);
						}
					}
					else if (pSymbolsByAddr)
					{
						CComPtr<IDiaSymbol> pSymbol;
						pSymbolsByAddr->symbolByRVA(address, &pSymbol);
						CComBSTR name;
						pSymbol->get_name(&name);
						if (FindAtomW(name) == 0)
						{
							WriteTo<STD_OUTPUT_HANDLE>("%ls @ %hu NONAME\r\n", name.m_str, ordinal);
							atom = AddAtomW(name);
						}
					}
				}
			}
			++i;
		}
	}
	CoUninitialize();
	LocalFree(ordinal_used);
	LocalFree(argv);
	return 0;
}
