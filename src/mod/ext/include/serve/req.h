typedef struct req {
    Type type;
    MemCh *m;
    i32 idx;
    util u;
    Iter routeIt;
} Req;

typedef struct req_handler_def {
    Type type;
    Req_Mk mk;
    Req_Setup setup;
    DoFunc handle;
    DoFunc finalize;
    Table *routes; /*<chain>*/
} HandlerDef;

typedef Req *(*Req_Mk)(MemCh *m);
typedef void (*Req_Setup)(struct serve *srv, Req *req);
Req *Make_Req(MemCh *m);
