enum span_flags {
    SLAB_ACTIVE = 1 << 1,
    SLAB_FULL = 1 << 2,
    SLAB_ALPHA = 1 << 3,
    SLAB_BRAVO = 1 << 4,
    SLAB_DELTA = 1 << 5,
    SLAB_ECHO = 1 << 6,
    SLAB_FOXTROT = 1 << 7,
};

typedef struct slab_props {
    Type type;
    word offset; 
    byte pos;
    i8 flags;
} SlabProps; 

typedef struct slab_meta {
    union {
        SlabProps props;
        i64 flags64;
    } vals;
} SlabMeta;

typedef Abstract Slab[];

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
