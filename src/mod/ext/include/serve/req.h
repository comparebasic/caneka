typedef struct req {
    Type type;
    MemCh *m;
    i32 idx;
    util u;
    Iter routeIt;
} Req;

typedef Req *(*Req_Mk)(MemCh *m);
typedef void (*Req_Setup)(MemCh *m, Req *req, struct serve *srv);
Req *Make_Req(MemCh *m);
