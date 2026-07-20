typedef struct task_core {
    Type type;
    i32 idx;
    ReqCrit *crit;
    Iter routeIt;
    struct {
        struct timespec start;
        struct timespec end;
    } metrics;
} TaskCore;

Req *Make_Req(MemCh *m);
