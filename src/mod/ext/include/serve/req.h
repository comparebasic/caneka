typedef struct req {
    Type type;
    i32 idx;
    ReqCrit *crit;
    Iter routeIt;
    struct {
        struct timespec start;
        struct timespec end;
    } metrics;
    void *source;
} Req;

Req *Req_Make(MemCh *m, void *source);
