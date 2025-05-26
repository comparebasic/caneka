typedef struct mem_ctx {
    RangeType type;
    i16 _;
    i16 guard;
    Iter it;
    Abstract *owner;
    MemPage *first;
} MemCh;

#define MemCh_SetToBase(m) ((m)->type.range = -((m)->type.range));
#define MemCh_SetFromBase(m) ((m)->type.range = abs((m)->type.range));

i64 MemChapterTotal();
i64 MemCh_MemCount(MemCh *m, i16 level);
void *MemCh_Alloc(MemCh *m, size_t sz);
void *MemCh_AllocOf(MemCh *m, size_t sz, cls typeOf);
i64 MemCh_Used(MemCh *m, i16 level);
i64 MemCh_Total(MemCh *m, i16 level);
status MemCh_WipeTemp(MemCh *m, i16 level);
status MemCh_FreeTemp(MemCh *m, i16 level);
status MemCh_Free(MemCh *m);
void *MemCh_GetPage(MemCh *m, void *addr, i32 *idx);
status MemCh_Setup(MemCh *m, MemPage *pg);
MemCh *MemCh_OnPage();
MemCh *MemCh_Make();
