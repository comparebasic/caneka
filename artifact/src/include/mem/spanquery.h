enum span_ops {
    SPAN_OP_GET = 1 << 8,
    SPAN_OP_SET = 1 << 9,
    SPAN_OP_REMOVE = 1 << 10,
    SPAN_OP_RESERVE = 1 << 11,
    SPAN_OP_RESIZE = 1 << 12,
};

typedef struct span_query {
    Type type;
    i32 idx;
    struct span *span;
    Abstract *value;
    SpanState stack[5];
} SpanQuery;

SpanState *SpanQuery_StateByDim(SpanQuery *sq, i8 dim);
status SpanQuery_Refresh(SpanQuery *sq);
void SpanQuery_Setup(struct span_query *sq, Span *p, status op, i32 idx);
status Span_Query(SpanQuery *sr);
slab *Slab_WhileExpanding(struct mem_slab **_sl);
