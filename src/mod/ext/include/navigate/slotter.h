enum slotter_flags {
    SLOTTER_SINGLE_IDX = 1 << 8,
    SLOTTER_REVERSE = 1 << 15, /* FLAG_ITER_REVERSE */
};

typedef util gobits;

typedef struct slotter {
    Type type;
    word _;
    gobits go;
    i32 slabIdx;
    Abstract *value;
    Iter it;
    Iter availableIt;
    Span/*<SlotterCrit>*/ *handlers;
    struct {
        struct timespec start;
        struct timespec now;
    } metrics;
} Slotter;

#define Slotter_GetMem(q) ((q)->itemsIt.p->m)
#define Slotter_Get(q) ((q)->value)
i32 Slotter_GetIdx(Slotter *q);
status Slotter_Set(Slotter *q, i32 idx, void *a);
i32 Slotter_Add(Slotter *q, void *a);
status Slotter_Remove(Slotter *q, i32 idx);
status Slotter_SetCriteria(Slotter *q, i32 critIdx, i32 idx, util **value);
util *Slotter_GetCriteria(Slotter *q, i32 critIdx, i32 idx);
status Slotter_Next(Slotter *q);
i32 Slotter_AddHandler(Slotter *q, struct slotter_crit *crit);
Slotter *Slotter_Make(MemCh *m);
status Slotter_Reset(Slotter *q);
