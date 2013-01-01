#include "stdafx.h"
#include "resource.h"
#include "TextBox.h"
#include "StyleDefinition.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CTextBox 


CTextBox::CTextBox(CWnd* pParent /*=NULL*/)
	: CDialog(CTextBox::IDD, pParent)
{
	ZeroMemory(&m_cds, sizeof m_cds);
	//{{AFX_DATA_INIT(CTextBox)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
}

CTextBox::~CTextBox()
{
}

void CTextBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTextBox)
	DDX_Control(pDX, 100, m_scintilla);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTextBox, CDialog)
	//{{AFX_MSG_MAP(CTextBox)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CTextBox 

static WINDOWPLACEMENT wp;

static BSTR NTAPI W2UTF8(PCWSTR wstr, int cchWideChar)
{
	int cbMultiByte = ::WideCharToMultiByte(CP_UTF8, 0, wstr, cchWideChar, 0, 0, 0, 0);
	PSTR pstr = (PSTR)::SysAllocStringByteLen(0, cbMultiByte);
	cbMultiByte = ::SysStringByteLen((BSTR)pstr);
	::WideCharToMultiByte(CP_UTF8, 0, wstr, cchWideChar, pstr, cbMultiByte, 0, 0);
	return (BSTR)pstr;
}

LRESULT CTextBox::SendEditor(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return m_scintilla.Call(uMsg, wParam, lParam);
}

void CTextBox::DefineMarker(int marker, int markerType, COLORREF fore, COLORREF back) {
	SendEditor(SCI_MARKERDEFINE, marker, markerType);
	SendEditor(SCI_MARKERSETFORE, marker, fore);
	SendEditor(SCI_MARKERSETBACK, marker, back);
}

void CTextBox::FoldChanged(int line, int levelNow, int levelPrev) {
	//Platform::DebugPrintf("Fold %d %x->%x\n", line, levelPrev, levelNow);
	if (levelNow & SC_FOLDLEVELHEADERFLAG) {
		if (!(levelPrev & SC_FOLDLEVELHEADERFLAG)) {
			// Adding a fold point.
			SendEditor(SCI_SETFOLDEXPANDED, line, 1);
			Expand(line, true, false, 0, levelPrev);
		}
	} else if (levelPrev & SC_FOLDLEVELHEADERFLAG) {
		//Platform::DebugPrintf("Fold removed %d-%d\n", line, SendEditor(SCI_GETLASTCHILD, line));
		if (!SendEditor(SCI_GETFOLDEXPANDED, line)) {
			// Removing the fold from one that has been contracted so should expand
			// otherwise lines are left invisible with no way to make them visible
			SendEditor(SCI_SETFOLDEXPANDED, line, 1);
			Expand(line, true, false, 0, levelPrev);
		}
	} else if (!(levelNow & SC_FOLDLEVELWHITEFLAG) &&
	        ((levelPrev & SC_FOLDLEVELNUMBERMASK) > (levelNow & SC_FOLDLEVELNUMBERMASK))) {
		// See if should still be hidden
		int parentLine = SendEditor(SCI_GETFOLDPARENT, line);
		if (parentLine < 0) {
			SendEditor(SCI_SHOWLINES, line, line);
		} else if (SendEditor(SCI_GETFOLDEXPANDED, parentLine) && SendEditor(SCI_GETLINEVISIBLE, parentLine)) {
			SendEditor(SCI_SHOWLINES, line, line);
		}
	}
}

void CTextBox::Expand(int &line, bool doExpand, bool force, int visLevels, int level) {
	int lineMaxSubord = SendEditor(SCI_GETLASTCHILD, line, level & SC_FOLDLEVELNUMBERMASK);
	line++;
	while (line <= lineMaxSubord) {
		if (force) {
			if (visLevels > 0)
				SendEditor(SCI_SHOWLINES, line, line);
			else
				SendEditor(SCI_HIDELINES, line, line);
		} else {
			if (doExpand)
				SendEditor(SCI_SHOWLINES, line, line);
		}
		int levelLine = level;
		if (levelLine == -1)
			levelLine = SendEditor(SCI_GETFOLDLEVEL, line);
		if (levelLine & SC_FOLDLEVELHEADERFLAG) {
			if (force) {
				if (visLevels > 1)
					SendEditor(SCI_SETFOLDEXPANDED, line, 1);
				else
					SendEditor(SCI_SETFOLDEXPANDED, line, 0);
				Expand(line, doExpand, force, visLevels - 1);
			} else {
				if (doExpand) {
					if (!SendEditor(SCI_GETFOLDEXPANDED, line))
						SendEditor(SCI_SETFOLDEXPANDED, line, 1);
					Expand(line, true, force, visLevels - 1);
				} else {
					Expand(line, false, force, visLevels - 1);
				}
			}
		} else {
			line++;
		}
	}
}

void CTextBox::FoldAll() {
	SendEditor(SCI_COLOURISE, 0, -1);
	int maxLine = SendEditor(SCI_GETLINECOUNT);
	bool expanding = true;
	for (int lineSeek = 0; lineSeek < maxLine; lineSeek++) {
		if (SendEditor(SCI_GETFOLDLEVEL, lineSeek) & SC_FOLDLEVELHEADERFLAG) {
			expanding = !SendEditor(SCI_GETFOLDEXPANDED, lineSeek);
			break;
		}
	}
	for (int line = 0; line < maxLine; line++) {
		int level = SendEditor(SCI_GETFOLDLEVEL, line);
		if ((level & SC_FOLDLEVELHEADERFLAG) &&
		        (SC_FOLDLEVELBASE == (level & SC_FOLDLEVELNUMBERMASK))) {
			if (expanding) {
				SendEditor(SCI_SETFOLDEXPANDED, line, 1);
				Expand(line, true, false, 0, level);
				line--;
			} else {
				int lineMaxSubord = SendEditor(SCI_GETLASTCHILD, line, -1);
				SendEditor(SCI_SETFOLDEXPANDED, line, 0);
				if (lineMaxSubord > line)
					SendEditor(SCI_HIDELINES, line + 1, lineMaxSubord);
			}
		}
	}
}

bool CTextBox::MarginClick(int position, int modifiers) {
	int lineClick = SendEditor(SCI_LINEFROMPOSITION, position);
	//Platform::DebugPrintf("Margin click %d %d %x\n", position, lineClick,
	//	SendEditor(SCI_GETFOLDLEVEL, lineClick) & SC_FOLDLEVELHEADERFLAG);
	if ((modifiers & SCMOD_SHIFT) && (modifiers & SCMOD_CTRL)) {
		FoldAll();
	} else {
		int levelClick = SendEditor(SCI_GETFOLDLEVEL, lineClick);
		if (levelClick & SC_FOLDLEVELHEADERFLAG) {
			if (modifiers & SCMOD_SHIFT) {
				EnsureAllChildrenVisible(lineClick, levelClick);
			} else if (modifiers & SCMOD_CTRL) {
				ToggleFoldRecursive(lineClick, levelClick);
			} else {
				// Toggle this line
				SendEditor(SCI_TOGGLEFOLD, lineClick);
			}
		}
	}
	return true;
}

void CTextBox::ToggleFoldRecursive(int line, int level) {
	if (SendEditor(SCI_GETFOLDEXPANDED, line)) {
		// Contract this line and all children
		SendEditor(SCI_SETFOLDEXPANDED, line, 0);
		Expand(line, false, true, 0, level);
	} else {
		// Expand this line and all children
		SendEditor(SCI_SETFOLDEXPANDED, line, 1);
		Expand(line, true, true, 100, level);
	}
}

void CTextBox::EnsureAllChildrenVisible(int line, int level) {
	// Ensure all children visible
	SendEditor(SCI_SETFOLDEXPANDED, line, 1);
	Expand(line, true, true, 100, level);
}

void CTextBox::Notify(SCNotification *notification)
{
	bool handled = false;
	//Platform::DebugPrintf("Notify %d\n", notification->nmhdr.code);
	switch (notification->nmhdr.code)
	{
	case SCN_MODIFIED:
		//if (notification->linesAdded && lineNumbers && lineNumbersExpand)
		//	SetLineNumberWidth();
		if (0 != (notification->modificationType & SC_MOD_CHANGEFOLD))
		{
			FoldChanged(notification->line,
					notification->foldLevelNow, notification->foldLevelPrev);
		}
		break;

	case SCN_MARGINCLICK:
		if (notification->margin == 2)
		{
			MarginClick(notification->position, notification->modifiers);
		}
		break;

	case SCN_ZOOM:
		//SetLineNumberWidth();
		break;
	}
}

BOOL CTextBox::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if (HICON hicon = AfxGetApp()->LoadIcon(IDD))
	{
		SetIcon(hicon, ICON_BIG);
	}

	m_scintilla.SetupDirectAccess();
	SendEditor(SCI_SETTABWIDTH, 4);

	CStyleDefinition rgsd[STYLE_MAX + 1];
	TCHAR szPath[MAX_PATH];
	::GetModuleFileName(0, szPath, MAX_PATH);
	::PathRenameExtension(szPath, _T(".ini"));
	rgsd->LoadStyles(szPath, _T("xml.styles"));
	rgsd->SetStyles(m_scintilla);

	enum
	{
		foldMargin = true,
		foldMarginWidth = 14,
		foldSymbols = 1,
	};
	//SendEditor(SCI_SETKEYWORDS, 0, (LPARAM)"test");

	SendEditor(SCI_SETPROPERTY, (WPARAM)"fold", (LPARAM)"1");
	SendEditor(SCI_SETPROPERTY, (WPARAM)"fold.html", (LPARAM)"1");
	SendEditor(SCI_SETPROPERTY, (WPARAM)"lexer.xml.allow.scripts", (LPARAM)"0");
	SendEditor(SCI_SETFOLDFLAGS, SC_FOLDFLAG_LINEAFTER_CONTRACTED);

	SendEditor(SCI_SETMODEVENTMASK, SC_MOD_CHANGEFOLD);

	// Create a margin column for the folding symbols
	SendEditor(SCI_SETMARGINTYPEN, 2, SC_MARGIN_SYMBOL);
	SendEditor(SCI_SETMARGINWIDTHN, 2, foldMargin ? foldMarginWidth : 0);
	SendEditor(SCI_SETMARGINMASKN, 2, SC_MASK_FOLDERS);
	SendEditor(SCI_SETMARGINSENSITIVEN, 2, 1);

	switch (foldSymbols)
	{
	case 0:
		// Arrow pointing right for contracted folders, arrow pointing down for expanded
		DefineMarker(SC_MARKNUM_FOLDEROPEN, SC_MARK_ARROWDOWN, RGB(0, 0, 0), RGB(0, 0, 0));
		DefineMarker(SC_MARKNUM_FOLDER, SC_MARK_ARROW, RGB(0, 0, 0), RGB(0, 0, 0));
		DefineMarker(SC_MARKNUM_FOLDERSUB, SC_MARK_EMPTY, RGB(0, 0, 0), RGB(0, 0, 0));
		DefineMarker(SC_MARKNUM_FOLDERTAIL, SC_MARK_EMPTY, RGB(0, 0, 0), RGB(0, 0, 0));
		DefineMarker(SC_MARKNUM_FOLDEREND, SC_MARK_EMPTY, RGB(0xff, 0xff, 0xff), RGB(0, 0, 0));
		DefineMarker(SC_MARKNUM_FOLDEROPENMID, SC_MARK_EMPTY, RGB(0xff, 0xff, 0xff), RGB(0, 0, 0));
		DefineMarker(SC_MARKNUM_FOLDERMIDTAIL, SC_MARK_EMPTY, RGB(0xff, 0xff, 0xff), RGB(0, 0, 0));
		break;
	case 1:
		// Plus for contracted folders, minus for expanded
		DefineMarker(SC_MARKNUM_FOLDEROPEN, SC_MARK_MINUS, RGB(0xff, 0xff, 0xff), RGB(0, 0, 0));
		DefineMarker(SC_MARKNUM_FOLDER, SC_MARK_PLUS, RGB(0xff, 0xff, 0xff), RGB(0, 0, 0));
		DefineMarker(SC_MARKNUM_FOLDERSUB, SC_MARK_EMPTY, RGB(0xff, 0xff, 0xff), RGB(0, 0, 0));
		DefineMarker(SC_MARKNUM_FOLDERTAIL, SC_MARK_EMPTY, RGB(0xff, 0xff, 0xff), RGB(0, 0, 0));
		DefineMarker(SC_MARKNUM_FOLDEREND, SC_MARK_EMPTY, RGB(0xff, 0xff, 0xff), RGB(0, 0, 0));
		DefineMarker(SC_MARKNUM_FOLDEROPENMID, SC_MARK_EMPTY, RGB(0xff, 0xff, 0xff), RGB(0, 0, 0));
		DefineMarker(SC_MARKNUM_FOLDERMIDTAIL, SC_MARK_EMPTY, RGB(0xff, 0xff, 0xff), RGB(0, 0, 0));
		break;
	case 2:
		// Like a flattened tree control using circular headers and curved joins
		DefineMarker(SC_MARKNUM_FOLDEROPEN, SC_MARK_CIRCLEMINUS, RGB(0xff, 0xff, 0xff), RGB(0x40, 0x40, 0x40));
		DefineMarker(SC_MARKNUM_FOLDER, SC_MARK_CIRCLEPLUS, RGB(0xff, 0xff, 0xff), RGB(0x40, 0x40, 0x40));
		DefineMarker(SC_MARKNUM_FOLDERSUB, SC_MARK_VLINE, RGB(0xff, 0xff, 0xff), RGB(0x40, 0x40, 0x40));
		DefineMarker(SC_MARKNUM_FOLDERTAIL, SC_MARK_LCORNERCURVE, RGB(0xff, 0xff, 0xff), RGB(0x40, 0x40, 0x40));
		DefineMarker(SC_MARKNUM_FOLDEREND, SC_MARK_CIRCLEPLUSCONNECTED, RGB(0xff, 0xff, 0xff), RGB(0x40, 0x40, 0x40));
		DefineMarker(SC_MARKNUM_FOLDEROPENMID, SC_MARK_CIRCLEMINUSCONNECTED, RGB(0xff, 0xff, 0xff), RGB(0x40, 0x40, 0x40));
		DefineMarker(SC_MARKNUM_FOLDERMIDTAIL, SC_MARK_TCORNERCURVE, RGB(0xff, 0xff, 0xff), RGB(0x40, 0x40, 0x40));
		break;
	case 3:
		// Like a flattened tree control using square headers
		DefineMarker(SC_MARKNUM_FOLDEROPEN, SC_MARK_BOXMINUS, RGB(0xff, 0xff, 0xff), RGB(0x80, 0x80, 0x80));
		DefineMarker(SC_MARKNUM_FOLDER, SC_MARK_BOXPLUS, RGB(0xff, 0xff, 0xff), RGB(0x80, 0x80, 0x80));
		DefineMarker(SC_MARKNUM_FOLDERSUB, SC_MARK_VLINE, RGB(0xff, 0xff, 0xff), RGB(0x80, 0x80, 0x80));
		DefineMarker(SC_MARKNUM_FOLDERTAIL, SC_MARK_LCORNER, RGB(0xff, 0xff, 0xff), RGB(0x80, 0x80, 0x80));
		DefineMarker(SC_MARKNUM_FOLDEREND, SC_MARK_BOXPLUSCONNECTED, RGB(0xff, 0xff, 0xff), RGB(0x80, 0x80, 0x80));
		DefineMarker(SC_MARKNUM_FOLDEROPENMID, SC_MARK_BOXMINUSCONNECTED, RGB(0xff, 0xff, 0xff), RGB(0x80, 0x80, 0x80));
		DefineMarker(SC_MARKNUM_FOLDERMIDTAIL, SC_MARK_TCORNER, RGB(0xff, 0xff, 0xff), RGB(0x80, 0x80, 0x80));
		break;
	}

	/*SendEditor(SCI_MARKERSETFORE, markerBookmark,
	           ColourOfProperty(props, "bookmark.fore", ColourDesired(0, 0, 0x7f)));
	SendEditor(SCI_MARKERSETBACK, markerBookmark,
	           ColourOfProperty(props, "bookmark.back", ColourDesired(0x80, 0xff, 0xff)));
	SendEditor(SCI_MARKERSETALPHA,
		allowAlpha ? props.GetInt("bookmark.alpha", SC_ALPHA_NOALPHA) : SC_ALPHA_NOALPHA);*/

	SendEditor(SCI_SETSCROLLWIDTH, 2000);
	SendEditor(SCI_SETSCROLLWIDTHTRACKING, 1);

	// Do these last as they force a style refresh
	SendEditor(SCI_SETHSCROLLBAR, 1);
	//SendEditor(SCI_SETENDATLASTLINE, 1);

	SendEditor(SCI_SETLEXER, SCLEX_XML);

	m_scintilla.SetFocus();
	switch (m_cds.dwData)
	{
	case SQL_WCHAR:
	case SQL_WVARCHAR:
	case SQL_WLONGVARCHAR:
		if (PSTR pstr = (PSTR)W2UTF8((PCWSTR)m_cds.lpData, (int)m_cds.cbData / 2))
		{
			m_scintilla.AddText(::SysStringByteLen((BSTR)pstr), pstr);
			::SysFreeString((BSTR)pstr);
		}
		break;
	default:
		m_scintilla.AddText((int)m_cds.cbData, (const char *)m_cds.lpData);
		break;
	}
	m_scintilla.SetSel(0, 0);
	if (wp.showCmd)
		SetWindowPlacement(&wp);
	else
		ShowWindow(SW_MAXIMIZE);
	return FALSE;
}

void CTextBox::OnOK()
{
	if (int cch = m_scintilla.GetTextLength())
	{
		*&m_text = ::SysAllocStringByteLen(0, cch);
		m_scintilla.GetText(cch + 1, (char *)*&m_text);
	}
	CDialog::OnOK();
}

void CTextBox::OnDestroy() 
{
	GetWindowPlacement(&wp);
	CDialog::OnDestroy();
}

void CTextBox::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	RepositionBars(0, 0xffff, 100);
}

BOOL CTextBox::PreTranslateMessage(MSG* pMsg) 
{
	switch (pMsg->message)
	{
	case WM_KEYDOWN:
		switch (pMsg->wParam)
		{
		case VK_ESCAPE:
			OnCancel();
			break;
		case 'W':
			if (GetKeyState(VK_CONTROL) < 0)
			{
				int mode = m_scintilla.GetWrapMode();
				m_scintilla.SetWrapMode(mode ^ SC_WRAP_WORD);
				return TRUE;
			}
			break;
		}
		break;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CTextBox::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	NMHDR *pNMHDR = (NMHDR *)lParam;
	if (pNMHDR->idFrom == 100)
		Notify(reinterpret_cast<SCNotification *>(pNMHDR));
	return CDialog::OnNotify(wParam, lParam, pResult);
}
