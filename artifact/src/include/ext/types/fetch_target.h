enum fetch_target_flags {
    FETCH_TARGET_ATT = 1 << 8,
    FETCH_TARGET_KEY = 1 << 9,
    FETCH_TARGET_IDX = 1 << 10,
    FETCH_TARGET_SELF = 1 << 11,
    FETCH_TARGET_ITER = 1 << 12,
    FETCH_TARGET_FUNC = 1 << 13,
    FETCH_TARGET_RESOLVED = 1 << 14,
};

typedef Abstract *(*FetchFunc)(struct fetch_target *target, Abstract *data, Abstract *source);

typedef struct fetch_target {
    Type type;
    Type objType;
    word _;
    i16 offset;
    i32 idx;
    Str *key;
    FetchFunc func;
} FetchTarget;

FetchTarget *FetchTarget_Make(MemCh *m);
FetchTarget *FetchTarget_MakeKey(MemCh *m, Str *key);
FetchTarget *FetchTarget_MakeAtt(MemCh *m, Str *att);
FetchTarget *FetchTarget_MakeFunc(MemCh *m, Str *key);
FetchTarget *FetchTarget_MakeIdx(MemCh *m, i32 idx);
