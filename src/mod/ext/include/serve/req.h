typedef struct req {
    Type type;
    MemCh *m;
    i32 idx;
    util u;
    Iter routeIt;
} Req;

Req *Make_Req(MemCh *m);
