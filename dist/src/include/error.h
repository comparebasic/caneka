void Cleanup(Abstract *a);

void *Error(char *msg);
void LogError(char *msg, ...);
void ExitError(int code, char *msg, ...);
void *_Fatal(char *msg, cls t, char *func, char *file, int line);
#ifdef LINUX
#define Fatal(msg, t) _Fatal((msg), (t), __func__, __FILE__, __LINE__)
#else
#define Fatal(msg, t) _Fatal((msg), (t), "unknown", __FILE__, __LINE__)
#endif
