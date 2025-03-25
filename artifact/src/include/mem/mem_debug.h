i64 MemCtx_Print(MemCtx *m, struct strvec *v, i32 fd, Abstract *a, cls type, boolean extended);
i64 MemBook_Print(MemCtx *m, struct strvec *v, i32 fd, Abstract *a, cls type, boolean extended);
i64 Span_Print(MemCtx *m, struct strvec *v, i32 fd, Abstract *a, cls type, boolean extended);
status Mem_DebugInit(MemCtx *m, struct lookup *lk);
