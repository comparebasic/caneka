typedef struct mem_ctx {
    RangeType type;
    i16 nextIdx;
    i16 nextCount;
    Iter it;
    Abstract *owner;
} MemCh;

i64 MemCh_MemCount(MemCh *m, i16 level);
i64 MemCh_Used(MemCh *m);
i64 MemCh_Total(MemCh *m, i16 level);
status MemCh_WipeTemp(MemCh *m, i16 level);
status MemCh_FreeTemp(MemCh *m, i16 level);
status MemCh_Free(MemCh *m);
void *MemCh_GetPage(MemCh *m, void *addr, i32 *idx);
status MemCh_Setup(MemCh *m, MemPage *pg);
MemCh *MemCh_OnPage();
MemCh *MemCh_Make();
