typedef struct lookup {
    word offset;
    Span *values;
    Abstract *arg;
} Lookup;

typedef status (*LookupPopulate)(MemCtx *m, Lookup *lk);

Lookup *Lookup_Make(MemCtx *m, word offset, LookupPopulate populate, Abstract *arg);
void *Lookup_Get(Lookup *lk, word type);
status Lookup_Add(MemCtx *m, Lookup *lk, word type, void *value);
