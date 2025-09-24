typedef struct mem_iter {
    Type type;
    MemCh *target;
    i32 slIdx;
    void *ptr;
    void *end;
} MemIter;



MemIter *MemIter_Make(MemCh *m, MemCh *target);
void MemIter_Init(MemIter *mit, MemCh *target);
status MemIter_Next(MemIter *mit);
Abstract *MemIter_Get(MemIter *mit);
