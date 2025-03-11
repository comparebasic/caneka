#define SPAN_LOCAL_MAX (SPAN_STRIDE-1)

enum span_ops {
    SPAN_OP_GET = 1,
    SPAN_OP_SET = 2,
    SPAN_OP_REMOVE = 3,
    SPAN_OP_RESERVE = 4,
    SPAN_OP_RESIZE = 5,
};

typedef struct span_state {
    slab *slab;
    word flags; /* active, full, etc. */
    i16 localIdx; 
    i16 offset;
    i32 increment;
    i8 dim;
} SpanState;

typedef struct span_query {
    Type type;
    struct mem_ctx *m;
    struct span *span;
    /* end SpanState */
    SpanState stack[SPAN_MAX_DIMS+1];
    i32 idx;
    Abstract *value;
    byte op;
    i8 dims;
    byte queryDim;
    i8 dimsNeeded;
} SpanQuery; /* sr */

SpanState *SpanQuery_SetStack(SpanQuery *sq, i8 dim);
SpanState *SpanQuery_StateByDim(SpanQuery *sq, i8 dim);
status SpanQuery_Refresh(SpanQuery *sq);
void SpanQuery_Setup(struct span_query *sq, Span *p, byte op, i32 idx);
status Span_Query(SpanQuery *sr);
slab *Slab_WhileExpanding(struct mem_slab **_sl);
