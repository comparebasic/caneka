#ifdef LINUX
#define FUNCNAME __func__
#define FILENAME __FILE__
#define LINENUMBER __LINE__
#else
#define FUNCNAME "unknown"
#define FILENAME __FILE__
#define LINENUMBER __LINE__
#endif

extern struct span *ErrorHandlerTable; /* Hashable -> Maker */
extern boolean _crashing;
extern boolean _error;

void Cleanup(Abstract *a);
void Fatal(char *func, char *file, int line, char *fmt, Abstract *args[]);
void ShowError(char *func, char *file, int line, char *fmt, Abstract *args[]);
void Error(MemCh *m, Abstract *a, char *func, char *file, int line, char *fmt, Abstract *args[]);
boolean IsZeroed(byte *b, size_t sz, char *func, char *file, int line);
