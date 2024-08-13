typedef struct slab_meta {
    struct {
        word max;
        byte dim;
        i8 flags;
    } metaType;
    int offset;
} SlabMeta;

typedef Abstract *Slab;

typedef struct span_slab {
    Slab *items;
    SlabMeta meta;
} SlabSlot;

typedef struct slab_result {
    Type type;
    MemCtx *m;
    struct span *span;
    Slab *slab;
    Slab *shelfSlab;
    int offset;
    int idx;
    Abstract *value;
    Slab *stack[8];
    byte op;
    byte dims;
    byte dimsNeeded;
    byte local_idx;
    byte level;
} SlabResult;

Slab* Slab_Alloc(MemCtx* m);
void *Slab_GetPtr(Slab *sl, int idx);
