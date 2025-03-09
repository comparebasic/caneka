#define SPAN_LOCAL_MAX (SPAN_STRIDE-1)

enum span_ops {
    SPAN_OP_GET = 1,
    SPAN_OP_SET = 2,
    SPAN_OP_REMOVE = 3,
    SPAN_OP_RESERVE = 4,
    SPAN_OP_RESIZE = 5,
};

typedef struct span_state {
    void *slab;
    word flags; /* active, full, etc. */
    i16 localIdx; 
    i16 offset;
    int increment;
    byte dim;
} SpanState;

typedef struct span_query {
    Type type;
    struct mem_ctx *m;
    struct span *span;
    /* end SpanState */
    SpanState stack[MAX_DIMS+1];
    int idx;
    Abstract *value;
    byte op;
    byte dims;
    byte queryDim;
    byte dimsNeeded;
} SpanQuery; /* sr */

SpanState *SpanQuery_SetStack(SpanQuery *sq, byte dim);
SpanState *SpanQuery_StateByDim(SpanQuery *sq, byte dim);
status SpanQuery_Refresh(SpanQuery *sq);
void SpanQuery_Setup(struct span_query *sq, Span *p, byte op, int idx);
