typedef struct req {
    Type type;
    MemCh *m;
    i32 idx;
    util *slot;
    Iter routeIt;
    struct {
        struct timespec start;
        struct timespec end;
    } metrics;
} Req;

Req *Make_Req(MemCh *m);
