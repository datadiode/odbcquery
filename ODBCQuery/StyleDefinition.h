class CStyleDefinition
{
	class Tokenizer;
public:
	TCHAR font[LF_FACESIZE];
	int size;
	COLORREF fore;
	COLORREF back;
	bool bold;
	bool italics;
	bool eolfilled;
	bool underlined;
	int caseForce;
	bool visible;
	bool changeable;
	enum { sdFont = 0x1, sdSize = 0x2, sdFore = 0x4, sdBack = 0x8,
		sdBold = 0x10, sdItalics = 0x20, sdEOLFilled = 0x40, sdUnderlined = 0x80,
		sdCaseForce = 0x100, sdVisible = 0x200, sdChangeable = 0x400 };
	int specified;
	CStyleDefinition();
	void Parse(LPCTSTR);
	void GetOneStyle(CScintillaCtrl &, int);
	void SetOneStyle(CScintillaCtrl &, int) const;
	void SetStyles(CScintillaCtrl &) const;
	void LoadStyles(LPCTSTR szPath, LPCTSTR szSection);
	void WriteFontHTML(CFile *, int);
	enum { htmlWysiwyg, htmlTabs, htmlSpaces };
	void WriteHTML(CFile *, CScintillaCtrl &, int = htmlWysiwyg);
};
