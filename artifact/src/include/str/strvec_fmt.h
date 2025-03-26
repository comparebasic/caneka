StrVec *StrVec_Fmt(MemCh *m, char *fmt, ...);
i64 StrVec_FmtAdd(MemCh *m, StrVec *v, i32 fd, char *fmt, ...);
i64 StrVec_FmtHandle(MemCh *m, StrVec *v, char *fmt, va_list args, i32 fd);
