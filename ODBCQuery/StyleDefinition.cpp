#include "stdafx.h"
#include "HtmlColor.h"
#include "StyleDefinition.h"

inline LRESULT CScintillaCtrl::Call(UINT message, WPARAM wParam, LPARAM lParam, BOOL bDirect)
{
  ASSERT(::IsWindow(m_hWnd)); //Window must be valid

  if (bDirect)
  {
    ASSERT(m_DirectFunction); //Direct function must be valid
    return (reinterpret_cast<SciFnDirect>(m_DirectFunction))(m_DirectPointer, message, wParam, lParam);
  }
  else
    return SendMessage(message, wParam, lParam);
}

class CStyleDefinition::Tokenizer
{
	Tokenizer(); // disallow construction
public:
	TCHAR text[1];
	const Tokenizer *Take(LPCTSTR noise) const
	{
		return this + StrCSpn(text, noise);
	}
	BOOL Match(const Tokenizer *p, LPCTSTR match) const
	{
		return StrCmpN(p->text, match, this - p) == 0;
	}
};

CStyleDefinition::CStyleDefinition()
: size(0), fore(0x000000), back(0xFFFFFF)
, bold(false), italics(false), eolfilled(false), underlined(false)
, caseForce(SC_CASE_MIXED), visible(true), changeable(true), specified(0)
{
	C_ASSERT(sizeof(Tokenizer) == sizeof(TCHAR)); // essential for pointer arith
}

void CStyleDefinition::Parse(LPCTSTR definition)
{
	int done = 0;
	const Tokenizer *p = reinterpret_cast<const Tokenizer *>(definition);
	while (*p->text) {
		const Tokenizer *q = p->Take(_T(":,"));
		const Tokenizer *r = q->Take(_T(","));
		// Find attribute name/value separator
		if (q->Match(p, "italics")) {
			specified |= sdItalics;
			italics = true;
		}
		else if (q->Match(p, "notitalics")) {
			specified |= sdItalics;
			italics = false;
		}
		else if (q->Match(p, "bold")) {
			specified |= sdBold;
			bold = true;
		}
		else if (q->Match(p, "notbold")) {
			specified |= sdBold;
			bold = false;
		}
		else if (q->Match(p, "font")) {
			specified |= sdFont;
			if (r > q)
			{
				int cch = r - q;
				if (cch > LF_FACESIZE)
					cch = LF_FACESIZE;
				lstrcpyn(font, q->text + 1, cch);
			}
		}
		else if (q->Match(p, "fore")) {
			specified |= sdFore;
			if (r > q) {
				WCHAR szColor[80];
				int cchColor = MultiByteToWideChar(CP_ACP, 0, q->text + 1, r - q - 1, szColor, 80);
				fore = ParseHtmlColorW(szColor, cchColor);
			}
		}
		else if (q->Match(p, "back")) {
			specified |= sdBack;
			if (r > q) {
				WCHAR szColor[80];
				int cchColor = MultiByteToWideChar(CP_ACP, 0, q->text + 1, r - q - 1, szColor, 80);
				back = ParseHtmlColorW(szColor, cchColor);
			}
		}
		else if (q->Match(p, "size")) {
			specified |= sdSize;
			if (r > q) {
				size = StrToInt(q->text + 1);
			}
		}
		else if (q->Match(p, "eolfilled")) {
			specified |= sdEOLFilled;
			eolfilled = true;
		}
		else if (q->Match(p, "noteolfilled")) {
			specified |= sdEOLFilled;
			eolfilled = false;
		}
		else if (q->Match(p, "underlined")) {
			specified |= sdUnderlined;
			underlined = true;
		}
		else if (q->Match(p, "notunderlined")) {
			specified |= sdUnderlined;
			underlined = false;
		}
		else if (q->Match(p, "case")) {
			specified |= sdCaseForce;
			caseForce = SC_CASE_MIXED;
			if (r > q) {
				if (q->text[1] == 'u') {
					caseForce = SC_CASE_UPPER;
				}
				else if (q->text[1] == 'l') {
					caseForce = SC_CASE_LOWER;
				}
			}
		}
		else if (q->Match(p, "visible")) {
			specified |= sdVisible;
			visible = true;
		}
		else if (q->Match(p, "notvisible")) {
			specified |= sdVisible;
			visible = false;
		}
		else if (q->Match(p, "changeable")) {
			specified |= sdChangeable;
			changeable = true;
		}
		else if (q->Match(p, "notchangeable")) {
			specified |= sdChangeable;
			changeable = false;
		}
		p = *r->text ? r + 1 : r;
	}
}

void CStyleDefinition::GetOneStyle(CScintillaCtrl &rCtrl, int style)
{
	specified = ~0;
	fore = rCtrl.Call(SCI_STYLEGETFORE, style, 0);
	back = rCtrl.Call(SCI_STYLEGETBACK, style, 0);
	bold = rCtrl.Call(SCI_STYLEGETBOLD, style, 0) != 0;
	italics = rCtrl.Call(SCI_STYLEGETITALIC, style, 0) != 0;
	eolfilled = rCtrl.Call(SCI_STYLEGETEOLFILLED, style, 0) != 0;
	size = rCtrl.Call(SCI_STYLEGETSIZE, style, 0);
	if (rCtrl.Call(SCI_STYLEGETFONT, style, 0) < LF_FACESIZE) {
		rCtrl.Call(SCI_STYLEGETFONT, style, (LPARAM)font);
	}
	underlined = rCtrl.Call(SCI_STYLEGETUNDERLINE, style, 0) != 0;
	caseForce = rCtrl.Call(SCI_STYLEGETCASE, style, 0);
	visible = rCtrl.Call(SCI_STYLEGETVISIBLE, style, 0) != 0;
	changeable = rCtrl.Call(SCI_STYLEGETCHANGEABLE, style, 0) != 0;
}

void CStyleDefinition::SetOneStyle(CScintillaCtrl &rCtrl, int style) const
{
	if (specified & sdItalics)
		rCtrl.StyleSetItalic(style, italics);
	if (specified & sdBold)
		rCtrl.StyleSetBold(style, bold);
	if (specified & sdFont)
		rCtrl.StyleSetFont(style, font);
	if (specified & sdFore)
		rCtrl.StyleSetFore(style, fore);
	if (specified & sdBack)
		rCtrl.StyleSetBack(style, back);
	if (specified & sdSize)
		rCtrl.StyleSetSize(style, size);
	if (specified & sdEOLFilled)
		rCtrl.StyleSetEOLFilled(style, eolfilled);
	if (specified & sdUnderlined)
		rCtrl.StyleSetUnderline(style, underlined);
	if (specified & sdCaseForce)
		rCtrl.StyleSetCase(style, caseForce);
	if (specified & sdVisible)
		rCtrl.StyleSetVisible(style, visible);
	if (specified & sdChangeable)
		rCtrl.StyleSetChangeable(style, changeable);
	int characterSet = 0;
	rCtrl.StyleSetCharacterSet(style, characterSet);
}

void CStyleDefinition::SetStyles(CScintillaCtrl &rCtrl) const
{
	if (this[STYLE_DEFAULT].specified)
		this[STYLE_DEFAULT].SetOneStyle(rCtrl, STYLE_DEFAULT);
	rCtrl.StyleClearAll();
	for (int style = 0 ; style <= STYLE_MAX ; ++style)
	{
		if (this[style].specified)
			this[style].SetOneStyle(rCtrl, style);
	}
}

static void fprintf(CFile *fp, const char *fmt, ...)
{
	char buffer[1024];
	int cb = wvsprintfA(buffer, fmt, va_list(&fmt + 1));
	fp->Write(buffer, cb);
}

static void fputc(char c, CFile *fp)
{
	fp->Write(&c, 1);
}

static void fputs(char *s, CFile *fp)
{
	fp->Write(s, lstrlenA(s));
}

void CStyleDefinition::LoadStyles(LPCTSTR szPath, LPCTSTR szSection)
{
	CFileStatus fs;
	if (CFile::GetStatus(szPath, fs))
	{
		CString strBuffer;
		LPTSTR pchBuffer = strBuffer.GetBuffer((DWORD)fs.m_size);
		TCHAR szEmpty[] = _T("");
		if (DWORD cch = GetPrivateProfileString(_T("$"), _T("font.base"),
			szEmpty, pchBuffer, fs.m_size, fs.m_szFullName))
		{
			this[STYLE_DEFAULT].Parse(pchBuffer);
		}
		DWORD cchBuffer = GetPrivateProfileSection(szSection, pchBuffer,
			(DWORD)fs.m_size, fs.m_szFullName);
		strBuffer.ReleaseBuffer(cchBuffer);
		while (*pchBuffer)
		{
			if (LPTSTR pchEquals = StrChr(pchBuffer, '='))
			{
				int style = StrToInt(pchBuffer);
				CString strStyle = pchEquals + 1;
				int i = 0;
				while ((i = strStyle.Find(_T("$("), i)) != -1)
				{
					int j = strStyle.Find(')', i);
					if (j != -1)
					{
						strStyle.SetAt(j, '\0');
						TCHAR szSubst[MAX_PATH];
						DWORD cchSubst = GetPrivateProfileString(_T("$"),
							LPCTSTR(strStyle) + i + 2,
							szEmpty, szSubst, MAX_PATH, fs.m_szFullName);
						strStyle.Delete(i, j - i + 1);
						strStyle.Insert(i, szSubst);
						i += cchSubst;
					}
					else
					{
						strStyle.ReleaseBuffer(i);
					}

				}
				this[style] = this[STYLE_DEFAULT];
				this[style].Parse(strStyle);
			}
			pchBuffer += lstrlen(pchBuffer) + 1;
		}
	}
}

void CStyleDefinition::WriteFontHTML(CFile *fp, int flags)
{
	if (flags & sdFont)
	{
		fprintf(fp, " face='%s'", font);
	}
	if (flags & sdFore)
	{
		fprintf(fp, " color='#%02x%02x%02x'", GetRValue(fore), GetGValue(fore), GetBValue(fore));
	}
}

//---------- Save to HTML ----------

void CStyleDefinition::WriteHTML(CFile *fp, CScintillaCtrl &acc, int mode)
{
	int tabSize = acc.GetTabWidth();
	int codePage = acc.GetCodePage();
	int i = acc.GetSelectionStart();
	int end = acc.GetSelectionEnd();
	//RemoveFindMarks();
	acc.Colourise(i, end);

	int style = STYLE_DEFAULT;
	if (this[style].specified == 0) {
		this[style].GetOneStyle(acc, style);
	}
	int inFont = 0;
	// <basefont>
	if (int flags = this[style].specified) {
		fputs("<basefont", fp);
		this[style].WriteFontHTML(fp, flags);
		fputc('>', fp);
	}
	if (mode != htmlWysiwyg) {
		fputs("<pre>\n", fp);
	}
	char ch = ' ';
	int column = 0;
	if (this[style].bold) {
		fputs("<b>", fp);
	}
	if (this[style].italics) {
		fputs("<i>", fp);
	}
	if (this[style].underlined) {
		fputs("<u>", fp);
	}
	for ( ; i < end ; ++i) {
		int stylePrev = style;
		char chPrev = ch;
		ch = (char)acc.GetCharAt(i);
		if (ch == '\n' && chPrev == '\r') {
			continue; // CR+LF line ending, skip the "extra" EOL char
		}
		style = acc.GetStyleAt(i);
		if (ch == '\r' || ch == '\n') {
			style = STYLE_DEFAULT;
		}
		int flags = 0;
		if (style != stylePrev) {
			if (this[style].specified == 0) {
				this[style].GetOneStyle(acc, style);
			}
			if (lstrcmp(this[style].font, this[stylePrev].font)) {
				flags |= sdFont;
			}
			if (this[style].fore != this[stylePrev].fore) {
				flags |= sdFore;
			}
			if (this[stylePrev].bold > this[style].bold) {
				fputs("</b>", fp);
			}
			if (this[stylePrev].italics > this[style].italics) {
				fputs("</i>", fp);
			}
			if (this[stylePrev].underlined > this[style].underlined) {
				fputs("</u>", fp);
			}
			if (flags) {
				if (inFont) {
					fputs("</font>", fp);
					flags = 0;
					if (lstrcmp(this[style].font, this[STYLE_DEFAULT].font)) {
						flags |= sdFont;
					}
					if (this[style].fore != this[STYLE_DEFAULT].fore) {
						flags |= sdFore;
					}
				}
				if (flags) {
					fputs("<font", fp);
					this[style].WriteFontHTML(fp, flags);
					fputc('>', fp);
				}
				inFont = flags;
			}
			if (this[stylePrev].underlined < this[style].underlined) {
				fputs("<u>", fp);
			}
			if (this[stylePrev].italics < this[style].italics) {
				fputs("<i>", fp);
			}
			if (this[stylePrev].bold < this[style].bold) {
				fputs("<b>", fp);
			}
		}
		if (ch == ' ') {
			if (mode == htmlWysiwyg && chPrev == ' ') {
				fputs("&nbsp", fp);
				ch = ';';
			}
			fputc(ch, fp);
			column++;
		} else if (ch == '\t') {
			int ts = tabSize - (column % tabSize);
			if (mode == htmlWysiwyg) {
				ch = chPrev;
				while (ts) {
					if (ch == ' ') {
						fputs("&nbsp", fp);
						ch = ';';
					} else {
						ch = ' ';
					}
					fputc(ch, fp);
					ts--;
					column++;
				}
			} else if (mode == htmlTabs) {
				fputc(ch, fp);
				column++;
			} else {
				while (ts) {
					fputc(' ', fp);
					ts--;
					column++;
				}
			}
		} else if (ch == '\r' || ch == '\n') {
			column = 0;
			if (mode == htmlWysiwyg) {
				fputs("<br>", fp);
				ch = ' ';
			}
			fputc('\n', fp);
		} else {
			switch (ch) {
			case '<':
				fputs("&lt;", fp);
				break;
			case '>':
				fputs("&gt;", fp);
				break;
			case '&':
				fputs("&amp;", fp);
				break;
			default:
				if ((ch & 0x80) && codePage != CP_UTF8) {
					WCHAR wch;
					MultiByteToWideChar(CP_ACP, 0, &ch, 1, &wch, 1);
					fprintf(fp, "&#%u;", (UINT)(WORD)wch);
				} else {
					fputc(ch, fp);
				}
			}
			column++;
		}
	}
	if (this[style].bold) {
		fputs("</b>", fp);
	}
	if (this[style].italics) {
		fputs("</i>", fp);
	}
	if (this[style].underlined) {
		fputs("</u>", fp);
	}
	if (inFont) {
		fputs("</font>", fp);
	}
	if (mode != htmlWysiwyg) {
		fputs("</pre>\n", fp);
	}
}
