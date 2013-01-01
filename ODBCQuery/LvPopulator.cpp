#include "stdafx.h"
#include "LvPopulator.h"

CLvPopulator::CLvPopulator(HWND hwnd)
: hwnd(hwnd), row(0), del(ListView_GetItemCount(hwnd))
{
	LVITEM::cchTextMax = 0;
	SNDMSG(hwnd, WM_SETREDRAW, 0, 0);
	ListView_DeleteAllItems(hwnd);
}

CLvPopulator *CLvPopulator::operator->()
{
	LVITEM::iItem = row++;
	LVITEM::iSubItem = 0;
	message = LVM_INSERTITEM;
	return this;
}

CLvPopulator *CLvPopulator::Append(LPCTSTR pszText)
{
	LVITEM::mask = LVIF_TEXT;
	LVITEM::pszText = const_cast<LPTSTR>(pszText);
	SNDMSG(hwnd, message, 0, (LPARAM)static_cast<LVITEM *>(this));
	++LVITEM::iSubItem;
	message = LVM_SETITEM;
	return this;
}

CLvPopulator *CLvPopulator::Append(int value)
{
	TCHAR szText[20];
	_itoa(value, szText, 10);
	Append(szText);
	return this;
}

CLvPopulator::~CLvPopulator()
{
	ListView_SetItemState(hwnd, 0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
	if (del == 0)
	{
		AdjustColumns(hwnd);
	}
	SNDMSG(hwnd, WM_SETREDRAW, 1, 0);
}

void CLvPopulator::AdjustColumns(HWND hwnd)
{
	HWND hwndHeader = ListView_GetHeader(hwnd);
	int n = Header_GetItemCount(hwndHeader);
	int i = 0;
	while (i < n)
	{
		ListView_SetColumnWidth(hwnd, i++, LVSCW_AUTOSIZE_USEHEADER);
	}
}

void CLvPopulator::InsertColumns(HWND hwnd, LV_COLUMN *pcol)
{
	int i = 0;
	while (*pcol->pszText)
	{
		ListView_InsertColumn(hwnd, i++, pcol);
		pcol->pszText += lstrlen(pcol->pszText) + 1;
	}
}
