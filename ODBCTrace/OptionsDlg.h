class COptionsDlg : public CFloatFlags
{
public:
	static BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
private:
	static void PopulateLbFilter(HWND);
	static void ApplyLbFilter(HWND);
	static COLORREF OnPbColor(HWND, COLORREF, LPCTSTR);
	static void OnPbBkgndColor(HWND);
	static BOOL OnTgSendToFile(HWND);
	static HWND hLbFilter;
};
