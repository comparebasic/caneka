extern struct buff *OutStream;
extern struct buff *ErrStream;
status Stream_Init(MemCh *m, i32 out, i32 err);
i64 Out(char *fmt, void *args[]);
