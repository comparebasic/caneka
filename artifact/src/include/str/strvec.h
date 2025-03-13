typedef struct strvec_cursor {
    Type type;
    i32 pos;
    StrVec *v;
    byte *ptr;
    byte *end;
    Iter it;
} StrVecCursor;

typedef struct strvec {
    Type type;
    i32 count;
    i64 total;
    Span *p;
} StrVec;

extern StrVecCursor _strVecCurs;

StrVec *StrVec_Make(MemCtx *m);
status StrVec_Add(StrVec *v, Str *s);
i64 StrVec_ToFd(StrVec *v, int fd);
Str *StrVec_ToStr(MemCtx *m, StrVec *v);
status StrVecCurs_Setup(StrVec *v, StrVecCursor *curs);
status StrVec_NextSlot(StrVec *v, StrVecCursor *curs);
