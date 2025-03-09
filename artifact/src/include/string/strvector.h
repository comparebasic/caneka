typedef struct strvec {
    Type type;
    i32 count;
    i64 total;
    Span *p;
} StrVec;

status StrVec_ToFd(StrVec *v, int fd);
Str *StrVec_ToStr(MemCtx *m, StrVec *v);
StrVec *StrVec_Make(MemCtx *m);
