typedef struct strvec_cursor {
    Type type;
    i32 idx;
    byte *ptr;
    byte *end;
    util slot;
    i64 total;
} StrVecCursor;

typedef struct strvec {
    Type type;
    i32 count;
    i64 total;
    i64 pos;
    Iter it;
} StrVec;

extern StrVecCursor _strVecCurs;

i64 StrVec_ToFd(StrVec *v, int fd);
Str *StrVec_ToStr(MemCtx *m, StrVec *v);
StrVec *StrVec_Make(MemCtx *m);
status StrVec_Add(StrVec *v, Str *s);
StrVec *StrVec_Fmt(MemCtx *m, char *fmt, ...);
i64 StrVec_FmtAdd(MemCtx *m, StrVec *v, i32 fd, char *fmt, ...);
status StrVecCurs_Setup(StrVec *v, StrVecCursor *curs);
status StrVec_NextSlot(StrVec *v, StrVecCursor *curs);
i64 StrVec_FmtHandle(MemCtx *m, StrVec *v, char *fmt, va_list args, i32 fd);
