/* not yet used, planned for optimizations */
enum slab_flags {
    SLAB_EMPTY = 1 << 8,
};

typedef slab {
    Type type;
    void *slots[SPAN_STRIDE];
} Slab;
