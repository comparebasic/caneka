enum span_flags {
    FLAG_SPAN_AUTOEXPAND = 1 << 16,
};

enum span_ops {
    SPAN_OP_GET = 1,
    SPAN_OP_SET = 2,
    SPAN_OP_REMOVE = 3,
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

#define Span_NextIdx(p) (p->max_idx+1)
