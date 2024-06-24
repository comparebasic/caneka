typedef struct span_slab {
    Type type;
    int local_idx;
    int increment;
    int level;
    int offset;
    Unit *items[SPAN_DIM_SIZE];
} Slab;

typedef struct slab_result {
    Type type;
    struct span *span;
    byte op;
    Slab *slab;
    Slab *shelfSlab;
    int offset;
    int idx;
    int dims;
    int dimsNeeded;
    int slotSize;
    int local_idx;
    int level;
    Unit *value;
} SlabResult;

Slab* Slab_Alloc(MemCtx* m, status flags);
