typedef struct serve_req {
    Type type;
    i32 idx;
    MemCh *m;
    util id;
    Handler *chain;
    Abstract *data;
    Abstract *source;
} Req;

Req *Req_Make(Handler *chain, Abstract *arg, Abstract *source);
status Req_Cycle(Req *req);
