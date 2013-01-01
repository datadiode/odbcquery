class TextReader
{
public:
	TextReader(ISequentialStream *);
	size_t readWord(char **, BYTE opAnd, BYTE opXor, size_t = 0);
	size_t readLine(char **, BYTE op);
	BYTE allocCtype(const char *);
private:
	char *copyWord(char *, const char *, BYTE opAnd, BYTE opXor, size_t);
	ISequentialStream *const pstm;
	ULONG index;
	ULONG ahead;
	char chunk[256];
	BYTE ctype[256];
};
