class CModuleDirectory
{
public:
	void *const hModule;
	IMAGE_DOS_HEADER *const pMZ;
	IMAGE_NT_HEADERS *const pPE;
	IMAGE_EXPORT_DIRECTORY *const pEAT;
	IMAGE_IMPORT_DESCRIPTOR *const pIAT;
	IMAGE_SECTION_HEADER *FindSection(DWORD);
	LPVOID GetAddress(DWORD);
	CModuleDirectory(HMODULE);
	IMAGE_IMPORT_DESCRIPTOR *FindIAT(LPCSTR);
	struct ATEntry
	{
		LPVOID lp;
		LPVOID Patch(LPVOID);
	};
	ATEntry *FindIATEntry(IMAGE_IMPORT_DESCRIPTOR *, LPVOID);
	ATEntry *FindEATEntry(LPCSTR);
	LPVOID operator->*(LPVOID lp) { return (LPBYTE)lp - (UINT_PTR)pMZ; }
};
