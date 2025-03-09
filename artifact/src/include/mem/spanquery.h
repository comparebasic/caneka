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

#define SpanQuery_SetStack(sq, dim, st) \
(do { \
    Span *p = sq->span; \
    slab *sl = NULL; \
    word localIdx = 0; \
    st = SpanQuery_StateByDim(sq, dim); \
    i32 increment = _increments[dim, SPAN_STRIDE]; \
    if(dim == p->dims){ \
        SpanState *st = SpanQuery_StateByDim(sq, p->dims); \
        sl = p->root; \
        localIdx = sq->idx / increment; \
        st->offset = localIdx * increment; \
    }else{ \
        SpanState *prev = SpanQuery_StateByDim(sq, dim+1); \
        localIdx = ((sq->idx - prev->offset) / increment); \
        word localMax = SPAN_STRIDE; \
        st->offset = prev->offset + increment*localIdx; \
        sl = (slab *)Slab_nextSlot(prev->slab, prev->localIdx); \
    } \
    st->slab = sl; \
    st->localIdx = localIdx; \
    st->dim = dim; \
    st->increment = increment; \
} while(0))

