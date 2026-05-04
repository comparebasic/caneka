enum mem_ite_flags {
    MEM_ITER_STREAM = 1 << 8,
};

typedef struct mem_ident {
    ;
} MemIdent;

typedef struct mem_iter {
    Type type;
    MemCh *m;
    struct {
        MemCh *target;
        void **arr;
    } input;
    struct {
        Type type;
        RangeType rtype;
        void *ptr;
        void *content;
        i32 slIdx;
        i32 idx;
    } current;
    i32 maxSlIdx;
    void *end;
} MemIter;

MemIter *MemIter_Make(MemCh *m, MemCh *target);
MemIter *MemIter_MakeFromArr(MemCh *m, void **arr, i32 maxSlIdx);
void MemIter_Init(MemIter *mit, MemCh *target);
void MemIter_InitArr(MemIter *mit, void **arr, i32 maxSlIdx);
status MemIter_Next(MemIter *mit);
MemIdent *MemIter_Get(MemIter *mit);
MemIdent *MemIter_CloneCurrent(MemCh *m, MemIter *mit);
