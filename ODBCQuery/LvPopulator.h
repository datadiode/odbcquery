class CLvPopulator : public LVITEM
{
	HWND hwnd;
	UINT message;
public:
	int row;
	int del;
	CLvPopulator(HWND);
	virtual CLvPopulator *operator->();
	virtual CLvPopulator *Append(LPCTSTR);
	virtual CLvPopulator *Append(int);
	~CLvPopulator();
	static void AdjustColumns(HWND);
	static void InsertColumns(HWND, LV_COLUMN *);
};
