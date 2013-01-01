class CInputBox : public CDialog
{
public:
	CInputBox(UINT nIDTemplate, CWnd *pParentWnd);
	~CInputBox();
protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
};
