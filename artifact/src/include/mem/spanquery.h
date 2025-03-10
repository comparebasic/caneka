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
    byte dim;
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
    byte dims;
    byte queryDim;
    byte dimsNeeded;
} SpanQuery; /* sr */

SpanState *SpanQuery_SetStack(SpanQuery *sq, byte dim);
SpanState *SpanQuery_StateByDim(SpanQuery *sq, byte dim);
status SpanQuery_Refresh(SpanQuery *sq);
void SpanQuery_Setup(struct span_query *sq, Span *p, byte op, i32 idx);
status Span_Query(SpanQuery *sr);

#define SpanQuery_SetStack(sq, d, st) \
do { \
    Span *p = (sq)->span; \
    slab *sl = NULL; \
    word localIdx = 0; \
    (st) = SpanQuery_StateByDim((sq), (d)); \
    i32 increment = _increments[(d)]; \
    if((d) == p->dims){ \
        SpanState *st = SpanQuery_StateByDim((sq), p->dims); \
        sl = p->root; \
        localIdx = (sq)->idx / increment; \
        st->offset = localIdx * increment; \
    }else{ \
        SpanState *prev = SpanQuery_StateByDim((sq), (d)+1); \
        localIdx = (((sq)->idx - prev->offset) / increment); \
        word localMax = SPAN_STRIDE; \
        st->offset = prev->offset + increment*localIdx; \
        void **ptr = (void **)prev->slab; \
        sl = (slab *)ptr+prev->localIdx; \
    } \
    st->slab = sl; \
    st->localIdx = localIdx; \
    st->dim = (d); \
    st->increment = increment; \
} while(0);
