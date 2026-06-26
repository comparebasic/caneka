typedef struct req {
    Type type;
    MemCh *m;
    i32 idx;
    util *slot;
    Iter routeIt;
} Req;

Req *Make_Req(MemCh *m);
