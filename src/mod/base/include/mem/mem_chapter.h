extern struct lookup *ExtFreeLookup;

#define SizeW(s) ((word)sizeof(s))

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
    Span *extFree;
#ifdef DEBUGSTACK
    Iter debugIt;
#endif
} MemCh;

void *MemCh_Alloc(MemCh *m, word sz);
void *MemCh_AllocOf(MemCh *m, word sz, cls typeOf);
void *MemCh_Realloc(MemCh *m, word s, void *orig, word origsize);
status MemCh_Free(MemCh *m);
status MemCh_FreeTemp(MemCh *m);
MemCh *MemCh_OnPage();
status MemCh_Setup(MemCh *m, MemPage *pg);
MemCh *MemCh_Make();

void MemCh_CountBytes(MemCh *m, i64 *count);
