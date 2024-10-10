/*
Slab
The slab element used by Span, the slab is almost entirely internal.

related: core/slab.c
related: include/span.h
*/
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
    int local_idx;
    Abstract *value;
    void *stack[MAX_DIMS+1];
    byte op;
    byte dims;
    byte dimsNeeded;
    byte level;
} SlabResult;

status Slab_setSlot(void *sl, struct span_def *def, int idx, void *value, size_t sz);
void *Slab_nextSlot(SlabResult *sr, struct span_def *def, int local_idx);
void **Slab_nextSlotPtr(SlabResult *sr, struct span_def *def, int local_idx);
void *Slab_valueAddr(SlabResult *sr, struct span_def *def, int local_idx);
void SlabResult_SetStack(SlabResult *sr, void *ptr, byte dim);
