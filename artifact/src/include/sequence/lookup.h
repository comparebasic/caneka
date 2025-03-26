typedef struct lookup {
    Type type;
    word offset;
    Span *values;
    Abstract *arg;
    int latest_idx;
} Lookup;

typedef struct lookup_config {
    i32 key;
    Abstract *a;
} LookupConfig;

typedef status (*LookupPopulate)(MemCh *m, Lookup *lk);

Lookup *Lookup_Make(MemCh *m, word offset, LookupPopulate populate, Abstract *arg);
Lookup *LookupInt_Make(MemCh *m, word offset, Abstract *arg);
void *Lookup_Get(Lookup *lk, word type);
status Lookup_Add(MemCh *m, Lookup *lk, word type, void *value);
status Lookup_Concat(MemCh *m, Lookup *lk, Lookup *add);
word Lookup_AbsFromIdx(Lookup *lk, word idx);
Lookup *Lookup_FromConfig(MemCh *m, LookupConfig *config, Abstract *arg);
int Lookup_GetKey(Lookup *lk, int idx);
