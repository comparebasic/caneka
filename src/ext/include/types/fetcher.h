enum fetcher_flags {
    FETCHER_VAR = 1 << 8,
    FETCHER_FOR = 1 << 9,
    FETCHER_WITH = 1 << 10,
    FETCHER_IF = 1 << 11,
    FETCHER_IFNOT = 1 << 12,
    FETCHER_END = 1 << 13,
    FETCHER_COMMAND = 1 << 14,
    FETCHER_TEMPL = 1 << 15,
};

typedef struct fetcher {
    Type type;
    union {
        Span *targets;
    } val;
} Fetcher;

void *Fetch(MemCh *m, Fetcher *fch, void *data, void *source);
Fetcher *Fetcher_Make(MemCh *m);
void *Fetch_FromPath(Fetcher *fch, void *data, void *source);
