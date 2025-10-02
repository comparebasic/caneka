enum mem_ite_flags {
    MEM_ITER_STREAM = 1 << 8,
};

typedef struct mem_iter {
    Type type;
    struct {
        MemCh *target;
        Abstract **arr;
    } input;
    i32 slIdx;
    i32 maxSlIdx;
    void *ptr;
    void *end;
} MemIter;

MemIter *MemIter_Make(MemCh *m, MemCh *target);
MemIter *MemIter_MakeFromArr(MemCh *m, Abstract **arr, i32 maxSlIdx);
void MemIter_Init(MemIter *mit, MemCh *target);
void MemIter_InitArr(MemIter *mit, Abstract **arr, i32 maxSlIdx);
status MemIter_Next(MemIter *mit);
Abstract *MemIter_Get(MemIter *mit);
