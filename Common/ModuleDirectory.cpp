/*/ModuleDirectory.cpp

Last edit: 2013-01-01 Jochen Neubeck

[The MIT license]

Copyright (c) 2006 Jochen Neubeck

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
#include "ModuleDirectory.h"

//This code is distilled from Anton Bassov's article at
//<http://www.codeproject.com/system/api_spying_hack.asp>

IMAGE_SECTION_HEADER *CModuleDirectory::FindSection(DWORD offset)
{
	IMAGE_SECTION_HEADER *p = IMAGE_FIRST_SECTION(pPE);
	IMAGE_SECTION_HEADER *q = p + pPE->FileHeader.NumberOfSections;
	while (p < q)
	{
		// Is the RVA within this section?
		if (offset >= p->VirtualAddress && offset < p->VirtualAddress + p->SizeOfRawData)
			return p;
		++p;
	}
	return 0;
}

LPVOID CModuleDirectory::GetAddress(DWORD offset)
{
	if (offset == 0)
		return NULL;
	if (hModule != pMZ)
		if (IMAGE_SECTION_HEADER *p = FindSection(offset))
			offset += p->PointerToRawData - p->VirtualAddress;
	return PCH(pMZ) + offset;
}

CModuleDirectory::CModuleDirectory(HMODULE hModule)
: hModule(hModule)
, pMZ(reinterpret_cast<IMAGE_DOS_HEADER *>(reinterpret_cast<UINT_PTR>(hModule) & ~1U))
, pPE(reinterpret_cast<IMAGE_NT_HEADERS *>(PCH(pMZ) + pMZ->e_lfanew))
, pEAT(reinterpret_cast<IMAGE_EXPORT_DIRECTORY *>(
	GetAddress(pPE->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress)))
, pIAT(reinterpret_cast<IMAGE_IMPORT_DESCRIPTOR *>(
	GetAddress(pPE->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress)))
{
	ASSERT(pMZ->e_magic == MAKEWORD('M','Z'));
	ASSERT(pPE->Signature == MAKELONG(MAKEWORD('P','E'), 0));
}

CModuleDirectory::ATEntry *CModuleDirectory::FindEATEntry(LPCSTR lpProcName)
{
	if (pEAT)
	{
		ULONG *rgAddressOfNames = (ULONG *)GetAddress(pEAT->AddressOfNames);
		WORD *rgNameIndexes = (WORD *)GetAddress(pEAT->AddressOfNameOrdinals);
		DWORD lower = 0;
		DWORD upper = pEAT->NumberOfNames;
		int cbProcName = lstrlenA(lpProcName) + 1;
		while (lower < upper)
		{
			DWORD match = (upper + lower) >> 1;
			char *candidate = (char *)GetAddress(rgAddressOfNames[match]);
			int cmp = memcmp(candidate, lpProcName, cbProcName);
			if (cmp >= 0)
				upper = match;
			if (cmp <= 0)
				lower = match + 1;
		}
		if (lower > upper)
			return (ATEntry *)GetAddress(pEAT->AddressOfFunctions) + rgNameIndexes[upper];
	}
	return 0;
}

IMAGE_IMPORT_DESCRIPTOR *CModuleDirectory::FindIAT(LPCSTR szName)
{
	if (IMAGE_IMPORT_DESCRIPTOR *pDescriptor = pIAT)
	{
		while (pDescriptor->FirstThunk)
		{
			char *candidate = (char *)GetAddress(pDescriptor->Name);
			if (lstrcmpiA(candidate, szName) == 0)
				return pDescriptor;
			++pDescriptor;
		}
	}
	return 0;
}

CModuleDirectory::ATEntry *CModuleDirectory::FindIATEntry(IMAGE_IMPORT_DESCRIPTOR *pDescriptor, LPVOID pImport)
{
	IMAGE_THUNK_DATA *pThunk = (IMAGE_THUNK_DATA *)GetAddress(pDescriptor->OriginalFirstThunk);
	ATEntry *pEntry = (ATEntry *)GetAddress(pDescriptor->FirstThunk);
	while (pThunk->u1.Function)
	{
		if (pEntry->lp == pImport)
			return pEntry;
		++pEntry;
		++pThunk;
	}
	return 0;
}

LPVOID CModuleDirectory::ATEntry::Patch(LPVOID lpNew)
{
	LPVOID lpOld = 0;
	DWORD dwOldProtect = 0;
	if (this && VirtualProtect(this, sizeof *this, PAGE_READWRITE, &dwOldProtect))
	{
		lpOld = lp;
		lp = lpNew;
		VirtualProtect(this, sizeof *this, dwOldProtect, &dwOldProtect);
	}
	return (LPVOID)lpOld;
}
