enum strvec_flags {
    STRVEC_PATH = 1 << 8,
};

typedef struct strvec {
    Type type;
    i32 _;
    i64 total;
    Span *p;
} StrVec;

i64 StrVec_ToFd(StrVec *v, int fd);
StrVec *StrVec_ReAlign(MemCh *m, StrVec *orig);
status StrVec_NextSlot(StrVec *v, struct cursor *curs);
i32 StrVec_GetIdx(StrVec *v, Str *s);
status StrVec_Add(StrVec *v, Str *s);
status StrVec_AddVec(StrVec *v, StrVec *v2);
status StrVec_AddBytes(MemCh *m, StrVec *v, byte *ptr, i64 length);
StrVec *StrVec_Make(MemCh *m);
StrVec *StrVec_From(MemCh *m, Str *s);
StrVec *StrVec_FromB64(MemCh *m, StrVec *v);
i64 StrVec_FfIter(Iter *it, i64 offset);
Abstract *StrVec_Nth(MemCh *m, StrVec *v, i32 n);
Abstract *StrVec_Clone(MemCh *m, Abstract *);
Str *StrVec_Str(MemCh *m, StrVec *v);
StrVec *StrVec_FromLongBytes(MemCh *m, byte *bytes, i32 length);
