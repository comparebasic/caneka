enum fetcher_flags {
    FETCHER_FOR = 1 << 8,
    FETCHER_WITH = 1 << 9,
    FETCHER_IF = 1 << 10,
    FETCHER_IFNOT = 1 << 11,
    FETCHER_END = 1 << 12,
    FETCHER_JUMP = 1 << 13,
};

typedef Abstract *(*FetchFunc)(struct fetcher *fch, Abstract *a, Abstract *source);

typedef struct fetcher {
    Type type;
    MemCh *m;
    StrVec *path;
    FetchTarget target;
} Fetcher;

Abstract *Fetch(Fetcher *fch, Abstract *a, Abstract *source);
Fetcher *Fetcher_Make(MemCh *m, StrVec *path);
Abstract *Fetch_FromPath(Fetcher *fch, Abstract *data, Abstract *source);
