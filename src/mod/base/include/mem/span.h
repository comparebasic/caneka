enum span_flags {
    SPAN_INLINE = 1 << 8,
    SPAN_EXPAND = 1 << 9,
    SPAN_QUEUE = 1 << 10,
    SPAN_TABLE = 1 << 11,
    SPAN_HAS_GAPS = 1 << 12,
    SPAN_ORDERED = 1 << 13,
};

typedef void *Slab[SPAN_STRIDE];

typedef struct span {
    Type type;
    RangeType range; /* type/dims */
    struct mem_ctx *m;
    i16 _;
    i16 memLevel;
    i64 count;
    i64 maxIdx;
    i64 size;
    Slab *root;
    /* first data: Slab */
} Span;

void *Span_Get(Span *p, i64 idx);
void Span_Set(Span *p, i64 idx, void *t);
void Span_Remove(Span *p, i64 idx);
i64 Span_Add(Span *p, void *t);

util Span_SetSlot(Span *p, i64 idx, util u);
util Span_GetSlot(Span *p, i64 idx);

Span *Span_Make(struct mem_ctx *m);
