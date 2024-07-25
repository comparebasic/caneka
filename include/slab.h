typedef struct span_slab {
    Type type;
    int increment;
    int offset;
    byte slotSize;
    Abstract *items[SPAN_DIM_SIZE];
} Slab;

typedef struct slab_result {
    Type type;
    struct span *span;
    byte op;
    Slab *slab;
    Slab *shelfSlab;
    int offset;
    int idx;
    byte dims;
    byte dimsNeeded;
    byte slotSize;
    byte local_idx;
    byte level;
    Abstract *value;
} SlabResult;

Slab* Slab_Alloc(MemCtx* m, status flags, byte slotSize);
void *Slab_GetPtr(Slab *sl, int idx);
