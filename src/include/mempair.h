typedef struct mem_persist {
    Type type;
    MemCtx *m;
    Span *map;
    String *path;
} MemPersist;

typedef struct mem_pair {
    Type type;
    void *slab;
    int offset;
} MemPair;

void *MemPair_GetAddr(MemPersist *mp, void *addr);
