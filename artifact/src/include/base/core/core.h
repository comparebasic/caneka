extern Stream *OutStream;
extern Stream *ErrStream;
status Core_Init(MemCh *m);
i64 Out(char *fmt, Abstract *args[]);
i64 Debug(char *fmt, Abstract *args[]);
