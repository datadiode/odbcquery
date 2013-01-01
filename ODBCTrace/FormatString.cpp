/*/FormatString.cpp

Copyright (c) 2006 Jochen Neubeck

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
#include "stdafx.h"

EXTERN_C STDAPI StreamPuts(ISequentialStream *pstm, LPCTSTR s)
{
	HRESULT hr;
	static const TCHAR t[] = TEXT("\r\n");
	while (int n = StrCSpn(s, t))
	{
		if FAILED(hr = pstm->Write(s, n * sizeof(TCHAR), 0))
			return hr;
		n = StrSpn(s += n, t);
		while (n)
		{
			if (*s == s[--n] && FAILED(hr = pstm->Write(t, 2 * sizeof(TCHAR), 0)))
				return hr;
			++s;
		}
	}
	return hr;
}

EXTERN_C STDAPI StreamVPrintf(ISequentialStream *piStream, UINT codepage, LPCTSTR fmt, va_list arglist, BOOL bTerminate)
{
	HRESULT hr;
	while (LPCTSTR pct = StrChr(fmt, '%'))
	{
		TCHAR ahead = pct[1];
		int i = 0;
		if (ahead == '%')
		{
			++pct;
		}
		if FAILED(hr = piStream->Write(fmt, (pct - fmt) * sizeof(TCHAR), 0))
			return hr;
		if (ahead != '%')
		{
			TCHAR fmt2[40], text[128];
			UINT_PTR arg = va_arg(arglist, UINT_PTR);
			i = StrCSpn(pct, TEXT("cCdsSuixX"));
			if (i < 39)
			{
				TCHAR hlDefault = sizeof(TCHAR) == sizeof(WCHAR) ? 'l' : 'h';
				switch (pct[i])
				{
				case 'S':
					hlDefault = sizeof(TCHAR) == sizeof(WCHAR) ? 'h' : 'l';
				case 's':
					if (arg)
					{
						int cchText;
						TCHAR hlOverride = pct[i - 1];
						switch (hlOverride != '%' ? hlOverride : hlDefault)
						{
#ifdef _UNICODE
						case 'h':
							cchText = lstrlenA((LPCSTR)arg);
							while (cchText)
							{
								// TODO: Adjust cchDone to preceding codepoint boundary
								WCHAR szWideChar[512 + 1];
								int cchDone = cchText < 512 ? cchText : 512;
								if (int cchWideChar = MultiByteToWideChar(codepage, 0, (LPCSTR)arg, cchDone, szWideChar, sizeof szWideChar))
								{
									if FAILED(hr = piStream->Write(szWideChar, cchWideChar * sizeof(WCHAR), 0))
										return hr;
								}
								cchText -= cchDone;
							}
							break;
						case 'l':
#else
						case 'l':
							cchText = lstrlenW((LPCWSTR)arg);
							while (cchText)
							{
								CHAR szMultiByte[6 * 512 + 1];
								int cchDone = cchText < 512 ? cchText : 512;
								if (int cchMultiByte = WideCharToMultiByte(codepage, 0, (LPCWSTR)arg, cchDone, szMultiByte, sizeof szMultiByte, 0, 0))
								{
									if FAILED(hr = piStream->Write(szMultiByte, cchMultiByte, 0))
										return hr;
								}
								cchText -= cchDone;
							}
							break;
						case 'h':
#endif
							cchText = lstrlen((LPCTSTR)arg);
							if (cchText)
							{
								if FAILED(hr = piStream->Write((LPCTSTR)arg, cchText * sizeof(TCHAR), 0))
									return hr;
							}
							break;
						default:
							if FAILED(hr = piStream->Write(pct, (i + 1) * sizeof(TCHAR), 0))
								return hr;
							break;
						}
					}
					else
					{
						static const TCHAR null[] = TEXT("(null)");
						if FAILED(hr = piStream->Write(null, sizeof(null) - sizeof(TCHAR), 0))
							return hr;
					}
					break;
				default:
					if (pct[i - 1] == 'h')
						arg &= 0xFFFF;
					StrCpyN(fmt2, pct, i + 2);
					if (int cchText = wsprintf(text, fmt2, arg))
					{
						if FAILED(hr = piStream->Write(text, cchText * sizeof(TCHAR), 0))
							return hr;
					}
					break;
				}
			}
		}
		fmt = pct + i + 1;
	}
	int cchText = lstrlen(fmt);
	if (bTerminate)
		++cchText;
	return piStream->Write(fmt, cchText * sizeof(TCHAR), 0);
}

EXTERN_C STDAPI_(LPTSTR) GlobalVPrintf(UINT codepage, LPCTSTR fmt, va_list arglist)
{
	IStream *piStream = 0;
	LPVOID pMem = 0;
	if SUCCEEDED(CreateStreamOnHGlobal(0, FALSE, &piStream))
	{
		StreamVPrintf(piStream, codepage, fmt, arglist, TRUE);
		HGLOBAL hMem = 0;
		if SUCCEEDED(GetHGlobalFromStream(piStream, &hMem))
		{
			pMem = GlobalLock(hMem);
			if (!pMem)
				GlobalFree(hMem);
		}
		piStream->Release();
	}
	return static_cast<LPTSTR>(pMem);
	// caller must GlobalFreePtr(pMem)
}
