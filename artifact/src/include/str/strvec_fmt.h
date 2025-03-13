StrVec *StrVec_Fmt(MemCtx *m, char *fmt, ...);
i64 StrVec_FmtAdd(MemCtx *m, StrVec *v, i32 fd, char *fmt, ...);
i64 StrVec_FmtHandle(MemCtx *m, StrVec *v, char *fmt, va_list args, i32 fd);
