/*/TextReader.cpp

Last edit: 2012-12-31 Jochen Neubeck

[The MIT license]

Copyright (c) 2012 Jochen Neubeck

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
#include "TextReader.h"

TextReader::TextReader(ISequentialStream *pstm)
	: pstm(pstm), index(0), ahead(0)
{
	ZeroMemory(ctype, sizeof ctype);
	ctype[0] = 1;
}

BYTE TextReader::allocCtype(const char *q)
{
	BYTE cookie = ctype[0];
	ctype[0] <<= 1;
	while (BYTE c = *q++)
	{
		ctype[c] |= cookie;
	}
	return cookie;
}

char *TextReader::copyWord(char *p, const char *q, BYTE opAnd, BYTE opXor, size_t n)
{
	while (n != 0)
	{
		char c = *q++;
		if (ctype[c] & opAnd ^ opXor)
			return p;
		*p++ = c;
		--n;
	}
	return NULL;
}

size_t TextReader::readWord(char **ps, BYTE opAnd, BYTE opXor, size_t n)
{
	char *s = *ps;
	do 
	{
		size_t i = n;
		n += ahead;
		s = (char *)CoTaskMemRealloc(s, n + 1);
		char *lower = s + i;
		if (char *upper = copyWord(lower, chunk + index, opAnd, opXor, ahead))
		{
			n = upper - lower;
			index += n;
			ahead -= n;
			n = upper - s;
			s = (char *)CoTaskMemRealloc(s, n + 1);
			break;
		}
		index = ahead = 0;
		pstm->Read(chunk, sizeof chunk, &ahead);
	} while (ahead != 0);
	s[n] = '\0';
	*ps = s;
	return n;
}

size_t TextReader::readLine(char **ps, BYTE op)
{
	size_t len = readWord(ps, op, 0);
	return readWord(ps, op, op, len);
}
