#define MEM_ITER_MAX (1 << 12)
enum mem_ite_flags {
    MEM_ITER_STREAM = 1 << 8,
};

typedef struct mem_ident {
    Type type;
    RangeType rtype;
    void *ptr;
    void *content;
    i32 slIdx;
    i32 idx;
} MemIdent;

typedef struct mem_iter {
    Type type;
    MemCh *m;
    struct {
        MemCh *target;
        void **arr;
    } input;
    MemIdent current;
    i32 maxSlIdx;
    void *end;
} MemIter;

MemIter *MemIter_Make(MemCh *m, MemCh *target);
MemIter *MemIter_MakeFromArr(MemCh *m, void **arr, i32 maxSlIdx);
void MemIter_Init(MemCh *m, MemIter *mit, MemCh *target);
void MemIter_InitArr(MemIter *mit, void **arr, i32 maxSlIdx);
status MemIter_Next(MemIter *mit);
MemIdent *MemIter_Get(MemIter *mit);
MemIdent *MemIter_CloneCurrent(MemCh *m, MemIter *mit);
struct span *MemIter_GetTable(MemCh *m, MemCh *target);
