typedef struct serve_req {
    Type type;
    MemCh *m;
    i32 fd;
    i32 idx;
    Handler *chain;
    Abstract *source;
} Req;

Req *Req_Make(Handler *chain, Abstract *arg, Abstract *source);
status Req_Run(Serve *ctx, Req *req);
