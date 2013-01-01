class CDBRow
{
public:
	class Item;
	class ItemBase
	{
	public:
		enum
		{
			fEnquoted = 1,
			fTruncate = 2,
		};
		ItemBase *operator->() { return this; }
		const ItemBase *operator->() const { return this; }
		virtual CString asString(LPCTSTR null = 0, UINT flags = 0) const = 0;
		virtual void GetRef(COPYDATASTRUCT &) const = 0;
		//virtual DECIMAL asNumber() const = 0;
		//virtual DATE asDate() const = 0;
		//virtual long asLong() const = 0;
		//virtual __int64 asInt64() const = 0;
		//virtual double asDouble() const = 0;
		virtual int Compare(const Item &) const = 0;
		virtual int Type() const = 0;
	};
	class Item : public ItemBase
	{
	private:
		const signed char *data;
		size_t size;
		virtual CString asString(LPCTSTR null = 0, UINT flags = 0) const;
		virtual void GetRef(COPYDATASTRUCT &) const;
		//virtual DECIMAL asNumber() const;
		//virtual DATE asDate() const;
		//virtual long asLong() const;
		//virtual __int64 asInt64() const;
		//virtual double asDouble() const;
		virtual int Compare(const Item &) const;
		virtual int Type() const;
	public:
		Item(const signed char *, size_t);
	};
	class Handle : public CObject
	{
		friend CDBRow;
	public:
		union
		{
			size_t offset[20];
			signed char data[80];
		};
		size_t Count() const
		{
			return offset[0] / sizeof offset[0] - 1;
		}
		class Item Item(size_t i) const
		{
			return class Item(data + offset[i], offset[i + 1] - offset[i]);
		}
		class Scan;
		CString asString(LPCTSTR sep = 0) const;
		CString asQuoted(LPCTSTR sep = 0) const;
		static int CALLBACK LVCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	} *handle;
	class ArrayBase;
	class Array;
	Handle *operator->() { return handle; }
	const Handle *operator->() const { return handle; }
	static Handle *Scan(CRecordset &);
	CDBRow(CRecordset &stmt): handle(Scan(stmt))
	{
	}
	~CDBRow()
	{
		delete handle;
	}
	CDBRow(Handle *handle = 0)
	: handle(handle)
	{
	}
	static const CString &Enquote(CString &);
	static CString MakeString(BSTR);
	static void WriteString(CFile *const, LPCTSTR);
	static void WriteBinary(CFile *const, const void *, UINT);
	static void MakeHtmlEntities(CString &);
	static UINT WriteHtcf(CFile *pf, UINT htcf = 0);
	static UINT WriteReport(CListCtrl &, CFile *pfText, CFile *pfHtml = 0, UINT htcf = 0, CFile *pfExcel = 0);
	static void CreateExcelDocument(CListCtrl &, CFile *);
private:
	CDBRow(const CDBRow &); // disallow copy construction
};
