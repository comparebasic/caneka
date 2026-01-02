enum fetch_target_flags {
    FETCH_TARGET_ATT = 1 << 8,
    FETCH_TARGET_KEY = 1 << 9,
    FETCH_TARGET_IDX = 1 << 10,
    FETCH_TARGET_ITER = 1 << 11,
    FETCH_TARGET_PROP = 1 << 12,
    FETCH_TARGET_FUNC = 1 << 13,
    FETCH_TARGET_RESOLVED = 1 << 14,
};

typedef void *(*FetchFunc)(MemCh *m, struct fetch_target *target, void *data);

typedef struct fetch_target {
    Type type;
    Type objType;
    RangeType *offsetType;
    i32 idx;
    util id;
    Str *key;
    FetchFunc func;
    IterApi *api;
} FetchTarget;

FetchTarget *FetchTarget_Make(MemCh *m);
FetchTarget *FetchTarget_MakeKey(MemCh *m, Str *key);
FetchTarget *FetchTarget_MakeAtt(MemCh *m, Str *att);
FetchTarget *FetchTarget_MakeFunc(MemCh *m, Str *key);
FetchTarget *FetchTarget_MakeIdx(MemCh *m, i32 idx);
FetchTarget *FetchTarget_MakeIter(MemCh *m);
FetchTarget *FetchTarget_MakeProp(MemCh *m, Str *method);


status FetchTarget_Resolve(MemCh *m, FetchTarget *tg, cls typeOf);
void *Fetch_Target(MemCh *m, FetchTarget *tg, void *value, void *source);
void *Fetch_ByKey(MemCh *m, void *a, Str *key, void *source);
void *Fetch_ByAtt(MemCh *m, void *a, Str *att, void *source);
void *Fetch_Prop(MemCh *m, void *a, Str *method, void *source);
Iter *Fetch_Iter(MemCh *m, void *a, void *source);
void *FetchFunc_SpanGetIter(MemCh *m,
    struct fetch_target *target, void *data, void *source);
