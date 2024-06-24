typedef struct span_slab {
    int local_idx;
    int increment;
    int stride;
    int level;
    int offset;
    Unit **items;
	i64 nallocated;
} Slab;

typedef struct slab_result {
    struct span *span;
    Slab *slab;
    boolean grow;
    int idx;
    int dims;
    int stride;
    int slotSize;
    int local_idx;
    int level;
    Unit *value;
} SlabResult;
