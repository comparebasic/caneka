enum span_flags {
    SPAN_INLINE = 1 << 8,
    SPAN_EXPAND = 1 << 9,
    SPAN_QUEUE = 1 << 10,
    SPAN_TABLE = 1 << 11,
    SPAN_HAS_GAPS = 1 << 12,
    SPAN_ORDERED = 1 << 13,
};

typedef void **Slab[SPAN_STRIDE];
typedef i8 NextSet[SPAN_STRIDE];

typedef struct span {
    Type type;
    RangeType range; /* type/dims */
    struct mem_ctx *m;
    i32 count;
    i32 maxIdx;
    Slab *root;
    /* first data: Slab */
} Span;

void *Span_Get(Span *p, i32 idx);
void Span_Set(Span *p, i32 idx, void *t);
void Span_Remove(Span *p, i32 idx);
i32 Span_Add(Span *p, void *t);

util Span_SetSlot(Span *p, i32 idx, util u);
util Span_GetSlot(Span *p, i32 idx);

Span *Span_Make(struct mem_ctx *m);
