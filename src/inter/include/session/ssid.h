typedef struct ssid_ctx {
    Type type;
    MemCh *m;
    Str *path;
    struct {
        i64 open;
        i64 closed;
        i64 count;
    } metrics;
} SsidCtx;

StrVec *Ssid_From(MemCh *m, StrVec *ua, microTime time);
