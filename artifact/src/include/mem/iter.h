enum span_ops {
    SPAN_OP_GET = 1 << 8,
    SPAN_OP_SET = 1 << 9,
    SPAN_OP_REMOVE = 1 << 10,
    SPAN_OP_RESERVE = 1 << 11,
    SPAN_OP_RESIZE = 1 << 12,
    FLAG_ITER_LAST = 1 << 13,
    FLAG_ITER_REVERSE = 1 << 14,
};

typedef struct iter {
    Type type;
    i32 idx;
    struct span *span;
    void *value;
    void *stack[SPAN_MAX_DIMS+1];
    i32 stackIdx[SPAN_MAX_DIMS+1];
    struct {
        i32 get;
        i32 set;
        i32 selected;
        i32 available;
    } metrics;
} Iter;

status Iter_Refresh(Iter *it);
void Iter_Setup(Iter *it, Span *p, status op, i32 idx);
slab *Slab_WhileExpanding(struct mem_slab **_sl);
static inline i32 Iter_SetStack(Iter *it, i8 dim, i32 offset);
status Iter_Next(Iter *it);
Abstract *Iter_Get(Iter *it);
status Iter_Reset(Iter *it);
void Iter_Init(Iter *it, Span *p);
status Iter_NextItem(Iter *it);
status Iter_Query(Iter *it);

