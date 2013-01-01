/*/HtmlColor.cpp

Last edit: 2013-01-01 Jochen Neubeck

[The MIT license]

Copyright (c) 2007 Jochen Neubeck

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
#include "HtmlColor.h"

STDAPI ParseHtmlColorW(LPCWSTR text, int cch)
{
	static const struct
	{
		LPCWSTR name;
		COLORREF rgb;
		} rgColorsByName[] = {
		L"activeborder",			0x80000000 | COLOR_ACTIVEBORDER,
		L"activecaption",			0x80000000 | COLOR_ACTIVECAPTION,
		L"aliceblue",				0xFFF8F0,
		L"antiquewhite",			0xD7EBFA,
		L"appworkspace",			0x80000000 | COLOR_APPWORKSPACE,
		L"aqua",					0xFFFF00,
		L"aquamarine",				0xD4FF7F,
		L"azure",					0xFFFFF0,
		L"background",				0x80000000 | COLOR_BACKGROUND,
		L"beige",					0xDCF5F5,
		L"bisque",					0xC4E4FF,
		L"black",					0x000000,
		L"blanchedalmond",			0xCDFFFF,
		L"blue",					0xFF0000,
		L"blueviolet",				0xE22B8A,
		L"brown",					0x2A2AA5,
		L"burlywood",				0x87B8DE,
		L"buttonface",				0x80000000 | COLOR_BTNFACE,
		L"buttonhighlight",			0x80000000 | COLOR_BTNHIGHLIGHT,
		L"buttonshadow",			0x80000000 | COLOR_BTNSHADOW,
		L"buttontext",				0x80000000 | COLOR_BTNTEXT,
		L"cadetblue",				0xA09E5F,
		L"captiontext",				0x80000000 | COLOR_CAPTIONTEXT,
		L"chartreuse",				0x00FF7F,
		L"chocolate",				0x1E69D2,
		L"coral",					0x507FFF,
		L"cornflowerblue",			0xED9564,
		L"cornsilk",				0xDCF8FF,
		L"crimson",					0x3C14DC,
		L"cyan",					0xFFFF00,
		L"darkblue",				0x8B0000,
		L"darkcyan",				0x8B8B00,
		L"darkgoldenrod",			0x0B86B8,
		L"darkgray",				0xA9A9A9,
		L"darkgreen",				0x006400,
		L"darkkhaki",				0x6BB7BD,
		L"darkmagenta",				0x8B008B,
		L"darkolivegreen",			0x2F6B55,
		L"darkorange",				0x008CFF,
		L"darkorchid",				0xCC3299,
		L"darkred",					0x00008B,
		L"darksalmon",				0x7A96E9,
		L"darkseagreen",			0x8FBC8F,
		L"darkslateblue",			0x8B3D48,
		L"darkslategray",			0x4F4F2F,
		L"darkturquoise",			0xD1CE00,
		L"darkviolet",				0xD30094,
		L"deeppink",				0x9314FF,
		L"deepskyblue",				0xFFBF00,
		L"dimgray",					0x696969,
		L"dodgerblue",				0xFF901E,
		L"firebrick",				0x2222B2,
		L"floralwhite",				0xF0FAFF,
		L"forestgreen",				0x228B22,
		L"fuchsia",					0xFF00FF,
		L"gainsboro",				0xDCDCDC,
		L"ghostwhite",				0xFFF8F8,
		L"gold",					0x00D7FF,
		L"goldenrod",				0x20A5DA,
		L"gray",					0x808080,
		L"graytext",				0x80000000 | COLOR_GRAYTEXT,
		L"green",					0x008000,
		L"greenyellow",				0x2FFFAD,
		L"highlight",				0x80000000 | COLOR_HIGHLIGHT,
		L"highlighttext",			0x80000000 | COLOR_HIGHLIGHTTEXT,
		L"honeydew",				0xF0FFF0,
		L"hotpink",					0xB469FF,
		L"inactiveborder",			0x80000000 | COLOR_INACTIVEBORDER,
		L"inactivecaption",			0x80000000 | COLOR_INACTIVECAPTION,
		L"inactivecaptiontext",		0x80000000 | COLOR_INACTIVECAPTIONTEXT,
		L"indianred",				0x5C5CCD,
		L"indigo",					0x82004B,
		L"infobackground",			0x80000000 | COLOR_INFOBK,
		L"infotext",				0x80000000 | COLOR_INFOTEXT,
		L"ivory",					0xF0F0FF,
		L"khaki",					0x8CE6F0,
		L"lavender",				0xFAE6E6,
		L"lavenderblush",			0xF5F0FF,
		L"lawngreen",				0x00FC7C,
		L"lemonchiffon",			0xCDFAFF,
		L"lightblue",				0xE6D8AD,
		L"lightcoral",				0x8080F0,
		L"lightcyan",				0xFFFFE0,
		L"lightgoldenrodyellow",	0xD2FAFA,
		L"lightgreen",				0x90EE90,
		L"lightgrey",				0xD3D3D3,
		L"lightpink",				0xC1B6FF,
		L"lightsalmon",				0x7AA0FF,
		L"lightseagreen",			0xAAB220,
		L"lightskyblue",			0xFACE87,
		L"lightslategray",			0x998877,
		L"lightsteelblue",			0xDEC4B0,
		L"lightyellow",				0xE0FFFF,
		L"lime",					0x00FF00,
		L"limegreen",				0x32CD32,
		L"linen",					0xE6F0FA,
		L"magenta",					0xFF00FF,
		L"maroon",					0x000080,
		L"mediumaquamarine",		0xAACD66,
		L"mediumblue",				0xCD0000,
		L"mediumorchid",			0xD355BA,
		L"mediumpurple",			0xDB7093,
		L"mediumseagreen",			0x71B33C,
		L"mediumslateblue",			0xEE687B,
		L"mediumspringgreen",		0x9AFA00,
		L"mediumturquoise",			0xCCD148,
		L"mediumvioletred",			0x8515C7,
		L"menu",					0x80000000 | COLOR_MENU,
		L"menutext",				0x80000000 | COLOR_MENUTEXT,
		L"midnightblue",			0x701919,
		L"mintcream",				0xFAFFF5,
		L"mistyrose",				0xE1E4FF,
		L"moccasin",				0xB5E4FF,
		L"navajowhite",				0xADDEFF,
		L"navy",					0x800000,
		L"oldlace",					0xE6F5FD,
		L"olive",					0x008080,
		L"olivedrab",				0x238E6B,
		L"orange",					0x00A5FF,
		L"orangered",				0x0045FF,
		L"orchid",					0xD670DA,
		L"palegoldenrod",			0xAAE8EE,
		L"palegreen",				0x98FB98,
		L"paleturquoise",			0xEEEEAF,
		L"palevioletred",			0x9370DB,
		L"papayawhip",				0xD5EFFF,
		L"peachpuff",				0xBDDBFF,
		L"peru",					0x3F85CD,
		L"pink",					0xCBC0FF,
		L"plum",					0xDDA0DD,
		L"powderblue",				0xE6E0B0,
		L"purple",					0x800080,
		L"red",						0x0000FF,
		L"rosybrown",				0x8F8FBC,
		L"royalblue",				0xE16941,
		L"saddlebrown",				0x13458B,
		L"salmon",					0x7280FA,
		L"sandybrown",				0x60A4F4,
		L"scrollbar",				0x80000000 | COLOR_SCROLLBAR,
		L"seagreen",				0x578B2E,
		L"seashell",				0xEEF5FF,
		L"sienna",					0x2D52A0,
		L"silver",					0xC0C0C0,
		L"skyblue",					0xEBCE87,
		L"slateblue",				0xCD5A6A,
		L"slategray",				0x908070,
		L"snow",					0xFAFAFF,
		L"springgreen",				0x7FFF00,
		L"steelblue",				0xB48246,
		L"tan",						0x8CB4D2,
		L"teal",					0x808000,
		L"thistle",					0xD8BFD8,
		L"threeddarkshadow",		0x80000000 | COLOR_3DDKSHADOW,
		L"threedface",				0x80000000 | COLOR_3DFACE,
		L"threedhighlight",			0x80000000 | COLOR_3DHIGHLIGHT,
		L"threedlightshadow",		0x80000000 | COLOR_3DLIGHT,
		L"threedshadow",			0x80000000 | COLOR_3DSHADOW,
		L"tomato",					0x4763FD,
		L"turquoise",				0xD0E040,
		L"violet",					0xEE82EE,
		L"wheat",					0xB3DEF5,
		L"white",					0xFFFFFF,
		L"whitesmoke",				0xF5F5F5,
		L"window",					0x80000000 | COLOR_WINDOW,
		L"windowframe",				0x80000000 | COLOR_WINDOWFRAME,
		L"windowtext",				0x80000000 | COLOR_WINDOWTEXT,
		L"yellow",					0x00FFFF,
		L"yellowgreen",				0x32CD9A
	};
#ifdef _DEBUG
	static LONG bSanityChecked = FALSE;
	if (InterlockedExchange(&bSanityChecked, TRUE) == FALSE)
	{
		int i = RTL_NUMBER_OF(rgColorsByName) - 1;
		while (int j = i--)
		{
			ASSERT(StrCmpIW(rgColorsByName[j].name, rgColorsByName[i].name) > 0);
		}
	}
#endif
	HRESULT hr = E_INVALIDARG;
	if (text)
	{
		int iVal = -1;
		if (*text == '#')
		{
			WCHAR wc[9];
			wc[0] = '0';
			wc[1] = 'x';
			wc[2] = wc[8] = '\0';
			switch (cch)
			{
			case 4:
				wc[2] = wc[3] = text[1];
				wc[4] = wc[5] = text[2];
				wc[6] = wc[7] = text[3];
				break;
			case 7:
				wc[2] = text[1];
				wc[3] = text[2];
				wc[4] = text[3];
				wc[5] = text[4];
				wc[6] = text[5];
				wc[7] = text[6];
				break;
			}
			if (StrToIntExW(wc, STIF_SUPPORT_HEX, &iVal))
			{
				hr = RGB(GetBValue(iVal), GetGValue(iVal), GetRValue(iVal));
			}
		}
		else
		{
			if (!StrToIntExW(text, STIF_DEFAULT, &iVal))
			{
				UINT lower = 0;
				UINT upper = RTL_NUMBER_OF(rgColorsByName);
				while (lower < upper)
				{
					UINT match = (upper + lower) >> 1;
					int cmp = StrCmpNIW(rgColorsByName[match].name, text, cch);
					if (cmp == 0 && rgColorsByName[match].name[cch])
						cmp = 1;
					if (cmp >= 0)
						upper = match;
					if (cmp <= 0)
						lower = match + 1;
				}
				if (lower > upper)
				{
					iVal = rgColorsByName[upper].rgb;
				}
			}
			if (iVal >= 0)
			{
				hr = iVal;
			}
			else if ((unsigned)iVal < 0x80000018)
			{
				hr = GetSysColor(iVal & 0x7FFFFFFF);
			}
		}
	}
	return hr;
}
