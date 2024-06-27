void *Error(char *msg);
void *_Fatal(char *msg, cls t, char *func, char *file, int line);
#define Fatal(msg, t) _Fatal((msg), (t), __func__, __FILE__, __LINE__)
