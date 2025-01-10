typedef struct mem_persist {
    Type type;
    MemCtx *m;
    struct span *map;
    struct string *path;
} MemPersist;

typedef struct mem_pair {
    Type type;
    void *slab;
    int offset;
} MemPair;

void *MemPair_GetAddr(MemPersist *mp, void *addr);
