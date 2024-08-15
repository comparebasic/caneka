typedef struct slab_meta {
    struct {
        word max;
        byte dim;
        i8 flags;
    } metaType;
    int offset;
} SlabMeta;

typedef struct span_slab {
    void *items;
    SlabMeta meta;
} SlabSlot;

typedef struct slab_result {
    Type type;
    MemCtx *m;
    struct span *span;
    void *slab;
    void *shelfSlab;
    int offset;
    int idx;
    Abstract *value;
    void *stack[8];
    byte op;
    byte dims;
    byte dimsNeeded;
    byte local_idx;
    byte level;
} SlabResult;

status Slab_setSlot(void *sl, struct span_def *def, int idx, void *value, size_t sz);
