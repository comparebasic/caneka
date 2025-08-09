typedef struct lookup {
    Type type;
    word offset;
    Span *values;
    int latest_idx;
} Lookup;

typedef struct lookup_config {
    i32 key;
    Abstract *a;
} LookupConfig;

typedef status (*LookupPopulate)(MemCh *m, Lookup *lk);
void *Lookup_Get(Lookup *lk, word type);
i64 Lookup_GetRaw(Lookup *lk, word type);
int Lookup_GetKey(Lookup *lk, int idx);
word Lookup_AbsFromIdx(Lookup *lk, word idx);
Lookup *Lookup_FromConfig(MemCh *m, LookupConfig *config);
status Lookup_Add(MemCh *m, Lookup *lk, word type, void *value);
status Lookup_AddRaw(MemCh *m, Lookup *lk, word type, i64 n);
status Lookup_Concat(MemCh *m, Lookup *lk, Lookup *add);
Lookup *LookupInt_Make(MemCh *m, word offset);
Lookup *Lookup_Make(MemCh *m, word offset);
