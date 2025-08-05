enum fetcher_flags {
    FETCHER_ATT = 1 << 8,
    FETCHER_KEY = 1 << 9,
    FETCHER_VALUE = 1 << 10,
    FETCHER_IDX = 1 << 11,
    FETCHER_NEST = 1 << 12,
    FETCHER_ITER = 1 << 13,
    FETCHER_FUNC = 1 << 14,
};

enum fetcher_op_flags {
    FETCHER_OP_FOR = 1 << 8,
    FETCHER_OP_WITH = 1 << 9,
    FETCHER_OP_IF = 1 << 10,
    FETCHER_OP_IFNOT = 1 << 11,
    FETCHER_OP_END = 1 << 12,
};

typedef Abstract *(*FetchFunc)(struct fetcher *fch, Abstract *a, Abstract *source);

typedef struct fetcher {
    Type type;
    Type objType;
    MemCh *m;
    i32 idx;
    StrVec *path;
    Abstract *key;
    FetchFunc func;
    Lookup *lk;
} Fetcher;

Abstract *Fetch(Fetcher *fch, Abstract *a, Abstract *source);
Fetcher *Fetcher_Make(MemCh *m, StrVec *path, i32 idx, Abstract *key, word flags, word op);
status Fetcher_SetOp(Fetcher *fch, word op);
