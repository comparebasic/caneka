enum memch_flags {
    MEMCH_BASE = 1 << 8,
    MEMCH_STASHED = 1 << 9,
};

typedef struct mem_ctx {
    Type type;
    i16 level;
    i16 guard;
    Iter it;
    void *owner;
    struct {
        i32 totalCeiling;
    } metrics;
} MemCh;

void *MemCh_Alloc(MemCh *m, size_t sz);
void *MemCh_AllocOf(MemCh *m, size_t sz, cls typeOf);
status MemCh_Free(MemCh *m);
status MemCh_FreeTemp(MemCh *m);
MemCh *MemCh_OnPage();
status MemCh_Setup(MemCh *m, MemPage *pg);
MemCh *MemCh_Make();
