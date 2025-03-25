enum span_ops {
    SPAN_OP_GET = 1 << 8,
    SPAN_OP_SET = 1 << 9,
    SPAN_OP_REMOVE = 1 << 10,
    SPAN_OP_RESERVE = 1 << 11,
    SPAN_OP_RESIZE = 1 << 12,
};

typedef struct span_iter {
    Type type;
    i32 idx;
    struct span *span;
    void *stack[SPAN_MAX_DIMS];
    i32 stackIdx[SPAN_MAX_DIMS];
} SpanIter;

status SpanIter_Refresh(SpanIter *sq);
void SpanIter_Setup(struct span_query *sq, Span *p, status op, i32 idx);
status Span_Query(SpanIter *sr);
slab *Slab_WhileExpanding(struct mem_slab **_sl);
static inline i32 SpanIter_SetStack(SpanIter *sq, i8 dim, i32 offset);
status SpanIter_Next(SpanIter *it);
Abstract *SpanIter_Get(SpanIter *it);
status SpanIter_Reset(SpanIter *it);
