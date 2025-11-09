extern struct buff *OutStream;
extern struct buff *ErrStream;
status Core_Init(MemCh *m);
i64 Out(char *fmt, void *args[]);
