enum memch_flags {
    MEMCH_BASE = 1 << 8,
    MEMCH_STASHED = 1 << 9,
};

typedef struct mem_ctx {
    Type type;
    i16 level;
    i16 guard;
    Iter it;
    Abstract *owner;
    MemPage *first;
} MemCh;

#define MemCh_SetToBase(m) ((m)->type.state |= MEMCH_BASE)
#define MemCh_SetFromBase(m) ((m)->type.state &= ~MEMCH_BASE)
#define MemCh_Incr(m) ((m)->level++)
#define MemCh_Decr(m) ((m)->level > 0 && (m)->level--)
#define MemCh_DecrFree(m) (((m)->level > 0) && \
    MemCh_FreeTemp((m), (m)->level) && ((m)->level--))

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
MemPage *MemCh_AddPage(MemCh *m, i16 level);
