typedef struct span_state {
    void *slab;
    word flags; /* active, full, etc. */
    word localIdx; 
    word offset;
} SpanState;

typedef struct span_query {
    Type type;
    MemCtx *m;
    struct span *span;
    /* end SpanState */
    SpanState stack[MAX_DIMS+1];
    int idx;
    Abstract *value;
    byte op;
    byte dims;
    byte dimsNeeded;
} SpanQuery; /* sr */

SpanState * SpanQuery_SetStack(SpanQuery *sq, byte dim, word set, word unset);
SpanState *SpanQuery_StateByDim(SpanQuery *sq, byte dim);
