#ifdef LINUX
#define FUNCNAME __func__
#define FILENAME __FILE__
#define LINENUMBER __LINE__
#else
#define FUNCNAME "unknown"
#define FILENAME __FILE__
#define LINENUMBER __LINE__
#endif

extern struct lookup *ErrorHandlers;
extern boolean _crashing;
extern boolean _error;

status Error_Init(struct mem_ctx *m);
void Cleanup(Abstract *a);
void Fatal(char *func, char *file, int line, char *fmt, Abstract *args[]);
void ShowError(char *func, char *file, int line, char *fmt, Abstract *args[]);
void Error(struct mem_ctx *m, Abstract *a, char *func, char *file, int line, char *fmt, Abstract *args[]);
boolean IsZeroed(byte *b, size_t sz, char *func, char *file, int line);
