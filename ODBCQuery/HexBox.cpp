#include "stdafx.h"
#include "resource.h"
#include "../frhed/heksedit.h"
#include "HexBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CHexBox 


CHexBox::CHexBox(CWnd* pParent /*=NULL*/)
	: CDialog(CHexBox::IDD, pParent)
{
	static const TCHAR szFileName[] = _T("heksedit.dll");
	if (::GetModuleHandle(szFileName) == 0)
		::LoadLibrary(szFileName);
	ZeroMemory(&m_cds, sizeof m_cds);
	//{{AFX_DATA_INIT(CHexBox)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
}

CHexBox::~CHexBox()
{
}

void CHexBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHexBox)
	DDX_Control(pDX, 100, m_heksedit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHexBox, CDialog)
	//{{AFX_MSG_MAP(CHexBox)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CHexBox 

static WINDOWPLACEMENT wp;

static void NTAPI MemCopy(void *p, const void *q, size_t n)
{
	if (p == 0)
		AfxThrowMemoryException();
	CopyMemory(p, q, n);
}

BOOL CHexBox::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if (HICON hicon = AfxGetApp()->LoadIcon(IDD))
	{
		SetIcon(hicon, ICON_BIG);
	}

	m_pif = reinterpret_cast<IHexEditorWindow *>(::GetWindowLongPtr(m_heksedit.m_hWnd, GWL_USERDATA));

	m_wndStatusBar.Create(&m_heksedit);
	m_wndStatusBar.SetParent(this);
	m_wndStatusBar.SetIndicators(0, 3);
	m_wndStatusBar.SetPaneInfo(0, 0, SBPS_STRETCH, 0);
	m_wndStatusBar.SetPaneInfo(1, 0, 0, 72);
	m_wndStatusBar.SetPaneInfo(2, 0, 0, 72);

	MemCopy(m_pif->get_buffer(m_cds.cbData), m_cds.lpData, m_cds.cbData);
	m_pif->resize_window();

	if (wp.showCmd)
		SetWindowPlacement(&wp);
	else
		ShowWindow(SW_MAXIMIZE);
	return TRUE;
}

void CHexBox::OnOK()
{
	if (int length = m_pif->get_length())
		MemCopy(*&m_text = ::SysAllocStringByteLen(0, length), m_pif->get_buffer(length), length);
	CDialog::OnOK();
}

void CHexBox::OnDestroy() 
{
	GetWindowPlacement(&wp);
	CDialog::OnDestroy();
}

void CHexBox::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	RepositionBars(0, 0xffff, 100);
}

BOOL CHexBox::PreTranslateMessage(MSG* pMsg) 
{
	switch (pMsg->message)
	{
	case WM_KEYDOWN:
		switch (pMsg->wParam)
		{
		case VK_ESCAPE:
			OnCancel();
			break;
		}
		break;
	}
	if (m_pif->translate_accelerator(pMsg))
		return TRUE;
	return CDialog::PreTranslateMessage(pMsg);
}
