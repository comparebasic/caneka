extern struct lookup *ExtFreeLookup;

#define SizeOf(s) ((quad)sizeof(s))

enum memch_flags {
    MEMCH_BASE = 1 << 8,
    MEMCH_STASHED = 1 << 9,
};

typedef struct mem_ctx {
    Type type;
    MemPage *page;
    Iter *backlog;
    MemCh *next;
#ifdef DEBUGSTACK
    Iter debugIt;
#endif
} MemCh;

void MemCh_CountBytes(MemCh *m, i64 *count);

void *MemCh_Alloc(MemCh *m, quad sz);
void *MemCh_Realloc(MemCh *m, quad s, void *orig, quad origsize);
status MemCh_Free(MemCh *m);
status MemCh_FreeTemp(MemCh *m);
MemCh *MemCh_OnPage();
status MemCh_Setup(MemCh *m, MemPage *pg);
MemCh *MemCh_Make();

