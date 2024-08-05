typedef struct iter {
    Type type;
    int idx;
    Span *values;
} Iter;

status Iter_Next(Iter *it);
Abstract *Iter_Get(Iter *it);
Iter *Iter_Make(MemCtx *m, Span *values);
