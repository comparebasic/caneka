typedef struct span_slabmin {
    flags_t flags;
    int local_idx;
    int increment;
    int level;
    int offset;
	i64 nallocated;
    Unit items[SLAB_MIN_SIZE];
} SlabMin;

typedef struct span_slab {
    Type type;
    int local_idx;
    int increment;
    int level;
    int offset;
	i64 nallocated;
    Unit items[SLAB_DIM_SIZE];
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
