extern struct buff *OutStream;
extern struct buff *ErrStream;
status Core_Direct(MemCh *m, i32 out, i32 err);
status Core_Init(MemCh *m);
i64 Out(char *fmt, void *args[]);
