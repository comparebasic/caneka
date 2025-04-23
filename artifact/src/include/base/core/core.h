extern Stream *OutStream;
extern Stream *ErrStream;
status Core_Init(MemCh *m);
i64 Out(char *fmt, void **args);
