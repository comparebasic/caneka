#define FUNCNAME ((char *)__func__)
#define FILENAME __FILE__
#define LINENUMBER __LINE__

extern struct lookup *ErrorHandlers;
extern boolean _crashing;
extern boolean _error;

status Error_Init(struct mem_ctx *m);
void Fatal(char *func, char *file, int line, char *fmt, Abstract *args[]);
void Error(struct mem_ctx *m,
    char *func, char *file, int line, char *fmt, Abstract *args[]);
boolean IsZeroed(struct mem_ctx *m, byte *b, size_t sz, char *func, char *file, int line);
