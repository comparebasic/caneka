enum span_flags {
    FLAG_SPAN_AUTOEXPAND = 1 << 16,
};

typedef struct span {
	Slab *slab;
	int nvalues;
    int slotSize;
    cls itemType;
    int itemSize;
	int ndims;
    int max_idx;
    /*
    Resolver *rslv;
    */
} Span;
