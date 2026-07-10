typedef struct req {
    Type type;
    MemCh *m;
    i32 idx;
    ReqCrit *crit;
    Iter routeIt;
    struct {
        struct timespec start;
        struct timespec end;
    } metrics;
} Req;

Req *Make_Req(MemCh *m);
