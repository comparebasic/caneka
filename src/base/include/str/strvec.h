enum strvec_flags {
    STRVEC_PATH = 1 << 8,
    STRVEC_ALIGNED = 1 << 9,
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
status StrVec_Incr(StrVec *v, i64 amount);
status StrVec_Decr(StrVec *v, i64 amount);
StrVec *StrVec_FromB64(MemCh *m, StrVec *v);
i64 StrVec_FfIter(Iter *it, i64 offset);
void *StrVec_Nth(MemCh *m, StrVec *v, i32 n);
void *StrVec_Clone(MemCh *m, void *);
Str *StrVec_Str(MemCh *m, StrVec *v);
Str *StrVec_ToStr(MemCh *m, StrVec *v, word length);
StrVec *StrVec_FromLongBytes(MemCh *m, byte *bytes, i32 length);
StrVec *StrVec_Copy(MemCh *m, StrVec *v);
StrVec *StrVec_StrVec(MemCh *m, void *a);
Str *StrVec_StrCombo(MemCh *m, void *a, void *b);
