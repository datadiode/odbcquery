/*/Tokenizer.cpp

Copyright (c) 2007 Jochen Neubeck

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
#include "Tokenizer.h"

const TCHAR CTokenizer::szTrim[] = TEXT(" \t\r\n");

CTokenizer *CTokenizer::EatSpace()
{
	return this ? this + StrSpn(szText, szTrim) : 0;
}

CTokenizer *CTokenizer::EatKeyword(LPCTSTR szText, BOOL fCaseSens)
{
	if (CTokenizer *p = EatSpace())
		if (int cch = p->LookAhead(-1))
			if (StrIsIntlEqual(fCaseSens, p->szText, szText, cch) && szText[cch] == '\0')
				return p + cch;
	return 0;
}

/**
 * @brief Look ahead for next token
 * depth = 0 (default) looks for entire expression
 * depth = -1 looks for keyword or identifier
 */
int CTokenizer::LookAhead(int depth)
{
	int quote = 0;
	int cch = 0;
recurse:
	switch (int c = szText[cch])
	{
	case '\'':
	case '"':
		if (quote == c || quote == 0)
			quote ^= c;
		break;
	case '(':
		if (!quote && ++depth <= 0)
			return cch;
		break;
	case ')':
		if (!quote && --depth < 0)
			return cch;
		break;
	case ',':
		if (!quote && !depth)
			return cch;
		break;
	case '\0':
		return cch;
	default:
		if (c <= 0x20 && !quote && depth < 0)
			return cch;
		break;
	}
	++cch;
	goto recurse;
}
