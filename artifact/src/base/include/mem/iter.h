enum span_ops {
    SPAN_OP_GET = 1 << 8,
    SPAN_OP_SET = 1 << 9,
    SPAN_OP_REMOVE = 1 << 10,
    SPAN_OP_RESERVE = 1 << 11,
    SPAN_OP_RESIZE = 1 << 12,
    SPAN_OP_ADD = 1 << 13,
    FLAG_ITER_CONTINUE = 1 << 14,
    FLAG_ITER_REVERSE = 1 << 15,
};

typedef struct iter {
    Type type;
    i32 idx;
    struct span *p;
    void *value;
    status maskFlags;
    status filterFlags;
    void *stack[SPAN_MAX_DIMS+1];
    i32 stackIdx[SPAN_MAX_DIMS+1];
    struct {
        i32 get;
        i32 set;
        i32 selected;
        i32 available;
    } metrics;
} Iter;

status Iter_Next(Iter *it);
status Iter_Prev(Iter *it);
status Iter_PrevRemove(Iter *it);
status Iter_Pop(Iter *it);
status Iter_Reset(Iter *it);
status Iter_Push(Iter *it, void *value);
void Iter_Init(Iter *it, Span *p);
void Iter_Setup(Iter *it, Span *p, status op, i32 idx);
Iter *Iter_Make(struct mem_ctx *m, Span *p);
status Iter_Set(Iter *it, void *value);
status Iter_Add(Iter *it, void *value);
status Iter_AddOn(Iter *it, void *value);
status Iter_Insert(Iter *it, i32 idx, void *value);
status Iter_SetByIdx(Iter *it, i32 idx, void *value);
void *Iter_Get(Iter *it);
void *Iter_Current(Iter *it);
status Iter_RemoveByIdx(Iter *it, i32 idx);
void *Iter_GetByIdx(Iter *it, i32 idx);
status Iter_GoToIdx(Iter *it, i32 idx);
status Iter_ExpandTo(Iter *it, i32 idx);
