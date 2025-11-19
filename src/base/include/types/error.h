#define FUNCNAME ((char *)__func__)
#define FILENAME __FILE__
#define LINENUMBER __LINE__

/*
#define SIGH_CATCH
*/

extern struct lookup *ErrorHandlers;
extern boolean _crashing;
extern boolean _error;
extern Abstract *ErrA;

status Error_Init(struct mem_ctx *m);
void Fatal(char *func, char *file, int line, char *fmt, void *args[]);
void Error(struct mem_ctx *m,
    char *func, char *file, int line, char *fmt, void *args[]);
boolean IsZeroed(struct mem_ctx *m, byte *b, size_t sz, char *func, char *file, int line);
