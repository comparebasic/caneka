typedef struct lookup {
    word offset;
    Span *values;
    Virtual *arg;
} Lookup;

typedef status (*LookupPopulate)(MemCtx *m, Lookup *lk);

Lookup *Lookup_Make(MemCtx *m, word offset, LookupPopulate populate, Virtual *arg);
