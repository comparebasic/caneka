enum fetcher_flags {
    FETCHER_ATT = 1 << 8,
    FETCHER_KEY = 1 << 9,
    FETCHER_VALUE = 1 << 10,
    FETCHER_IDX = 1 << 11,
    FETCHER_NEST = 1 << 12,
    FETCHER_ITER = 1 << 13,
    FETCHER_FUNC = 1 << 14,
};

typedef Abstract *(*FetchFunc)(Fetcher *fch, Abstract *a, Abstract *source);

typedef struct fetcher {
    Type type;
    Type objType;
    MemCh *m;
    i32 idx;
    Abstract *key;
    FetchFunc func;
    Lookup *lk;
}

Abstract *Fetch(Fetcher *fch, Abstract *a, Abstract *source);
Abstract *Fetcher_Make(MemCh *m, cls typeOf, i32 idx, Abstract *key, word flags);
