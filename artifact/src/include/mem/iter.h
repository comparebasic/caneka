typedef struct iter {
    Type type;
    int idx;
    Span *values;
} Iter;

enum iter_flags {
    FLAG_ITER_LAST = 1 << 10,
    FLAG_ITER_REVERSE = 1 << 11,
};

status Iter_Next(Iter *it);
Abstract *Iter_Get(Iter *it);
Iter *Iter_Init(Iter *it, Span *values);
Iter *Iter_InitReverse(Iter *it, Span *values);
Iter *Iter_Make(struct mem_ctx *m, Span *values);
status Iter_Reset(Iter *it);
