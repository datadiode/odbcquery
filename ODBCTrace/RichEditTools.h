LPTSTR NTAPI GetLineAtChar(HWND, LONG);

// InfoTip
HWND NTAPI CreateInfoTip(HWND);
void NTAPI ResizeInfoTip(REQRESIZE *);

// Editing
BOOL NTAPI JoinLines(HWND, CHARRANGE *, BOOL bAllowUndo);
void NTAPI JoinSelectedLines(HWND, BOOL bAllowUndo);
void NTAPI PasteAsSingleLine(HWND, BOOL bAllowUndo);
