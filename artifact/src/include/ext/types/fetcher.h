enum fetcher_flags {
    FETCHER_FOR = 1 << 8,
    FETCHER_WITH = 1 << 9,
    FETCHER_IF = 1 << 10,
    FETCHER_IFNOT = 1 << 11,
    FETCHER_END = 1 << 12,
    FETCHER_JUMP = 1 << 13,
};

typedef struct fetcher {
    Type type;
    Span *targets;
} Fetcher;

Abstract *Fetch(MemCh *m, Fetcher *fch, Abstract *data, Abstract *source);
Fetcher *Fetcher_Make(MemCh *m);
Abstract *Fetch_FromPath(Fetcher *fch, Abstract *data, Abstract *source);
Abstract *Fetch_FromOffset(MemCh *m, Abstract *a, i16 offset, cls typeOf);
