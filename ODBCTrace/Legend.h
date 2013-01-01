#define FMT_PX "0x%08lx"
#define FMT_HX "0x%04hx"
#define FMT_GX "0x%08x"
#define FMT_LX "0x%08lx"
#define FMT_HS "%hs"
#define FMT_LS "%ls"

struct Legend
{
	char text[48];
	Legend(short &, const char *(*)(int) = 0);
	Legend(short *, const char *(*)(int) = 0);
	Legend(unsigned short &, const char *(*)(int) = 0);
	Legend(unsigned short *, const char *(*)(int) = 0);
	Legend(long &, const char *(*)(int) = 0);
	Legend(long *, const char *(*)(int) = 0);
	Legend(unsigned long &, const char *(*)(int) = 0);
	Legend(unsigned long *, const char *(*)(int) = 0);
	// callback library
	static const char *SqlType(int);
	static const char *SqlCType(int);
	static const char *SqlDriverCompletion(int);
	static const char *SqlCompletionType(int);
	static const char *SqlStmtAttr(int);
	static const char *SqlStmtOption(int);
	static const char *SqlConnectAttr(int);
	static const char *SqlConnectOption(int);
	static const char *SqlFreeStmtOption(int);
	static const char *SqlSetPosOperation(int);
	static const char *SqlSetPosLockType(int);
	static const char *SqlInputOutputType(int);
	static const char *SqlAttrOdbcCursors(int);
};
