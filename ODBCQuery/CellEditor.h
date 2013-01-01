extern const TCHAR CellEditor_EditClassName[];

HWND NTAPI CellEditor_CreateEdit(HWND);
BSTR NTAPI CellEditor_GetCellText(HWND, int, int);
HWND NTAPI CellEditor_ShowEdit(HWND, BOOL);
void NTAPI CellEditor_SelectCell(HWND, int, int);
void NTAPI CellEditor_SubclassListView(HWND);
