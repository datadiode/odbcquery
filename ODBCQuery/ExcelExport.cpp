#include "stdafx.h"
#include "DatabaseEx.h"
#include "RecordsetEx.h"

enum RecordType
{
	DefaultColor = 0x7fff,
	BOFRecord = 0x0809,
	EOFRecord = 0x0A,
	FontRecord = 0x0231,
	FormatRecord = 0x001E,
	NumberFormat = 0x041E,
	LabelRecord = 0x0204,
	WindowProtectRecord = 0x0019,
	XFRecord = 0x0243,
	HeaderRecord = 0x0014,
	FooterRecord = 0x0015,
	ExtendedRecord = 0x0243,
	StyleRecord = 0x0293,
	CodepageRecord = 0x0042,
	NumberRecord = 0x0203,
	ColumnInfoRecord = 0x007D
};

static void BIFF_WriteBOF(CFile *pFile)
{
	static const WORD data[] =
	{
		BOFRecord, 8, 0, 0x10, 0, 0
	};
	pFile->Write(data, sizeof data);
}

static void BIFF_WriteEOF(CFile *pFile)
{
	static const WORD data[] =
	{
		EOFRecord, 0
	};
	pFile->Write(data, sizeof data);
}

static void BIFF_WriteFormat(CFile *pFile, LPCSTR format)
{
	BYTE len = static_cast<BYTE>(strlen(format));
	static const WORD data[] =
	{
		NumberFormat, 5 + len, 0xA5, len
	};
	pFile->Write(data, sizeof data);
	BYTE grbit = 0;
	pFile->Write(&grbit, sizeof grbit);
	pFile->Write(format, len);
}

static void BIFF_WriteColumnInfo(CFile *pFile, int col, int width)
{
	WORD data[] =
	{
		ColumnInfoRecord, 12,
		static_cast<WORD>(col),
		static_cast<WORD>(col),
		static_cast<WORD>(width),
		15, 0, 0
	};
	pFile->Write(data, sizeof data);
}

static void BIFF_WriteCellValue(CFile *pFile, int row, int col, WORD fmt, double value)
{
	WORD data[] =
	{
		NumberRecord,
		static_cast<WORD>(6 + sizeof value),
		static_cast<WORD>(row),
		static_cast<WORD>(col),
		fmt
	};
	pFile->Write(data, sizeof data);
	pFile->Write(&value, sizeof value);
}

static void BIFF_WriteCellValue(CFile *pFile, int row, int col, WORD fmt, LPCSTR str)
{
	size_t len = strlen(str);
	WORD data[] =
	{
		LabelRecord,
		static_cast<WORD>(8 + len),
		static_cast<WORD>(row),
		static_cast<WORD>(col),
		fmt,
		static_cast<WORD>(len)
	};
	pFile->Write(data, sizeof data);
	pFile->Write(str, len);
}

static void BIFF_WriteCellValue(CFile *pFile, int row, int col,
	WORD fmt, const LPCSTR str, const CODBCFieldInfo &fieldInfo)
{
	switch (fieldInfo.m_nSQLType)
	{
	case SQL_BIT:
	case SQL_TINYINT:
	case SQL_INTEGER:
	case SQL_SMALLINT:
	case SQL_REAL:
	case SQL_DOUBLE:
	case SQL_NUMERIC:
	case SQL_DECIMAL:
		BIFF_WriteCellValue(pFile, row, col, fmt, atof(str));
		break;
	default:
		BIFF_WriteCellValue(pFile, row, col, fmt, str);
		break;
	}
}

void BIFF_WriteReport(CListCtrl *pLv, CFile *pFile)
{
	CODBCFieldInfo *rgODBCFieldInfos = reinterpret_cast
		<CODBCFieldInfo *>(::GetWindowLong(pLv->m_hWnd, GWL_USERDATA));
	HWND hHd = ListView_GetHeader(pLv->m_hWnd);
	int nCols = Header_GetItemCount(hHd);
	CWordArray rgFormatIndex;
	rgFormatIndex.SetSize(nCols);
	WORD wFormatIndex = 0;
	int cxChar = pLv->GetStringWidth(_T("0"));
	TCHAR szText[INFOTIPSIZE];
	LVITEM item;
	item.pszText = szText;
	item.cchTextMax = _countof(szText);
	LVCOLUMN lvc;
	lvc.mask = LVCF_TEXT | LVCF_WIDTH;
	lvc.pszText = szText;
	lvc.cchTextMax = _countof(szText);
	BIFF_WriteBOF(pFile);
	for (item.iSubItem = 0 ; item.iSubItem < nCols ; ++item.iSubItem)
	{
		const CODBCFieldInfo &fieldInfo = rgODBCFieldInfos[item.iSubItem];
		CString s = _T("0");
		CRecordsetEx::FixScale(fieldInfo, s);
		if (s.GetLength() > 1)
		{
			//rgFormatIndex[item.iSubItem] = wFormatIndex++;
			BIFF_WriteFormat(pFile, s);
		}
	}
	int iRow = 0;
	for (item.iSubItem = 0 ; item.iSubItem < nCols ; ++item.iSubItem)
	{
		if (pLv->GetColumn(item.iSubItem, &lvc))
		{
			BIFF_WriteColumnInfo(pFile, item.iSubItem, lvc.cx * 256 / cxChar);
			BIFF_WriteCellValue(pFile, iRow, item.iSubItem, 0, szText);
		}
	}
	item.iItem = -1;
	while ((item.iItem = pLv->GetNextItem(item.iItem, LVNI_SELECTED)) != -1)
	{
		++iRow;
		item.mask = LVIF_TEXT | LVIF_PARAM;
		for (item.iSubItem = 0 ; item.iSubItem < nCols ; ++item.iSubItem)
		{
			if (pLv->GetItem(&item) && item.lParam != 0)
			{
				const CODBCFieldInfo &fieldInfo = rgODBCFieldInfos[item.iSubItem];
				BIFF_WriteCellValue(pFile, iRow, item.iSubItem,
					rgFormatIndex[item.iSubItem], szText, fieldInfo);
			}
			item.mask = LVIF_TEXT;
		}
	}
	BIFF_WriteEOF(pFile);
}
