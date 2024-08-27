typedef struct lookup {
    word offset;
    Span *values;
    Abstract *arg;
    int latest_idx;
} Lookup;

typedef struct lookup_config {
    i32 key;
    Abstract *a;
} LookupConfig;

typedef status (*LookupPopulate)(MemCtx *m, Lookup *lk);

Lookup *Lookup_Make(MemCtx *m, word offset, LookupPopulate populate, Abstract *arg);
Lookup *LookupInt_Make(MemCtx *m, word offset, Abstract *arg);
void *Lookup_Get(Lookup *lk, word type);
status Lookup_Add(MemCtx *m, Lookup *lk, word type, void *value);
word Lookup_AbsFromIdx(Lookup *lk, word idx);
Lookup *Lookup_FromConfig(MemCtx *m, LookupConfig *config, Abstract *arg);
