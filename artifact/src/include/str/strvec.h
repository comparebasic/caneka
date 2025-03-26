typedef struct strvec {
    Type type;
    i32 count;
    i64 total;
    Span *p;
} StrVec;

StrVec *StrVec_Make(MemCh *m);
status StrVec_Add(StrVec *v, Str *s);
i64 StrVec_ToFd(StrVec *v, int fd);
Str *StrVec_ToStr(MemCh *m, StrVec *v);
status StrVecCurs_Setup(StrVec *v, struct strvec_cursor *curs);
status StrVec_NextSlot(StrVec *v, struct strvec_cursor *curs);
status StrVec_AddVec(StrVec *v, StrVec *v2);
i32 StrVec_GetIdx(StrVec *v, Str *s);
