enum iter_flags {
    ITER_GET_GET = 1 << 8,
    ITER_SET = 1 << 9,
    ITER_REMOVE = 1 << 10,
    ITER_RESERVE = 1 << 11,
    ITER_ADD = 1 << 12,
    ITER_REVERSE = 1 << 13,
};

typedef struct iter {
    Type type;
    RangeType range;
    struct span *p;
    void *value;
    Slab **stack;
    i8 *stackIdx;
    i64 idx;
    i64 selected;
} Iter;

extern Iter *IT;

status Iter_AddSpanRev(Iter *it, Span *p);
status Iter_AddSpan(Iter *it, Span *p);

void Iter_Init(Iter *it, Span *p);
status Iter_Next(Iter *it);
status Iter_Prev(Iter *it);

void Iter_Pop(Iter *it);
void Iter_Reset(Iter *it);
void Iter_GoToIdx(Iter *it, i32 idx);

void Iter_Add(Iter *it, void *value);
void Iter_Set(Iter *it, i32 idx, void *value);
void Iter_Insert(Iter *it, i32 idx, void *value);
void Iter_Push(Iter *it, void *value);
void Iter_Remove(Iter *it, i32 idx);

Iter *Iter_Make(struct mem_ctx *m, Span *p);
