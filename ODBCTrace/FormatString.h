EXTERN_C STDAPI StreamPuts(ISequentialStream *, LPCTSTR);
EXTERN_C STDAPI StreamVPrintf(ISequentialStream *, UINT, LPCTSTR, va_list, BOOL bTerminate = FALSE);
EXTERN_C STDAPI_(LPTSTR) GlobalVPrintf(UINT, LPCTSTR, va_list);
