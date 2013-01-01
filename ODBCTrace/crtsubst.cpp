#include "stdafx.h"

void *operator new(unsigned int cb)
{
	return CoTaskMemAlloc(cb);
}

void operator delete(void *pv)
{
	CoTaskMemFree(pv);
}

/*extern "C" int _purecall(void)
{
	int response = MessageBox(0, "_purecall", 0, MB_TOPMOST|MB_ICONSTOP|MB_OKCANCEL);
	if (response == IDCANCEL)
		ExitProcess(3);
	return 0;
}*/
