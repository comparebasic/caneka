typedef struct strvec {
    Type type;
    i32 count;
    i64 total;
    Iter it;
} StrVec;

i64 StrVec_ToFd(StrVec *v, int fd);
Str *StrVec_ToStr(MemCtx *m, StrVec *v);
StrVec *StrVec_Make(MemCtx *m);
status StrVec_Add(StrVec *v, Str *s);
StrVec *StrVec_Fmt(MemCtx *m, char *fmt, ...);
i64 StrVec_FmtAdd(MemCtx *m, StrVec *v, char *fmt, ...);
