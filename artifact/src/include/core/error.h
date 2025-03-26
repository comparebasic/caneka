void Cleanup(Abstract *a);

void *Error(char *msg);
void LogError(char *msg, ...);
void ExitError(int code, char *msg, ...);
void *Fatal(i32 fd, char *func, char *file, int line, char *fmt, ...);
#ifdef LINUX
#define FUNCNAME __func__
#define FILENAME __FILE__
#define LINENUMBER __LINE__
#else
#define FUNCNAME "unknown"
#define FILENAME __FILE__
#define LINENUMBER __LINE__
#endif
