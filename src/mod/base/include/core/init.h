extern struct buff *OutStream;
extern struct buff *ErrStream;
extern struct buff *RandStream;
status Core_Direct(struct mem_ctx *m, i32 out, i32 err);
status Core_Init(struct mem_ctx *m);
i64 Out(char *fmt, void *args[]);
