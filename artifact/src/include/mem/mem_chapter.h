typedef struct mem_slab {
    Type type;
    i16 level;
    i16 remaining;
    void *bytes;
} MemSlab;

typedef struct mem_ctx {
    RangeType type;
    i16 nextIdx;
    i16 nextCount;
    Iter it;
    Abstract *owner;
} MemCh;

void *MemCh_Alloc(MemCh *m, size_t s);
void *MemCh_AllocTemp(MemCh *m, size_t s, i16 level);
void *MemCh_Realloc(MemCh *m, size_t s, void *orig, size_t origsize);
MemCh *MemCh_Make();
status MemCh_Free(MemCh *m);
status MemCh_FreeTemp(MemCh *m, i16 level);
i64 MemCh_Used(MemCh *m);
MemCh *MemCh_OnPage(void *page);
status MemCh_Setup(MemCh *m, MemSlab *sl);
void *MemCh_GetSlab(MemCh *m, void *addr, i32 *idx);
MemSlab *MemSlab_Attach(MemCh *m, i16 level);
void *MemSlab_Alloc(MemSlab *sl, word sz);
MemSlab *MemSlab_Make(MemCh *m, i16 level);
#define MemSlab_Available(sl) (sl)->remaining
i64 MemCh_MemCount(MemCh *m, i16 level);
i64 MemCh_Total(MemCh *m, i16 level);
#define MemSlab_Taken(sl) (((word)MEM_SLAB_SIZE) - (sl)->remaining)
#define MemCh_SetToBase(m) ((m)->type.range = -((m)->type.range));
#define MemCh_SetFromBase(m) ((m)->type.range = abs((m)->type.range));
i64 MemChapterCount();
status MemCh_ReserveSpanExpand(MemCh *m, MemSlab *sl, word nextIdx);
