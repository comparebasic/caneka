eunum span_flags {
    SLAB_ACTIVE 1 << 1,
    SLAB_FULL 1 << 2,
    SLAB_ALPHA 1 << 3,
    SLAB_BRAVO 1 << 4,
    SLAB_DELTA 1 << 5,
    SLAB_ECHO 1 << 6,
    SLAB_FOXTROT 1 << 7,
};

struct slab_props {
    Type type;
    word offset; 
    byte pos;
    i8 flags;
}; 

struct slab_index {
    i64 flags64;
}; 

struct slab_meta {
    union {
        slab_props;
        slab_index;
    } vals;
} SlabMeta;

typedef struct span_slab {
    Abstract **items;
    SpabMeta meta;
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
    Slab *stack[8];
} SlabResult;

Slab* Slab_Alloc(MemCtx* m, status flags, byte slotSize);
void *Slab_GetPtr(Slab *sl, int idx);
