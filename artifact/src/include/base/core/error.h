#ifdef LINUX
#define FUNCNAME __func__
#define FILENAME __FILE__
#define LINENUMBER __LINE__
#else
#define FUNCNAME "unknown"
#define FILENAME __FILE__
#define LINENUMBER __LINE__
#endif

void Cleanup(Abstract *a);
void *Error(char *msg);
void *Fatal(char *func, char *file, int line, char *fmt, Abstract *args[]);
