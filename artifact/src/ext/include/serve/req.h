typedef struct serve_req {
    Type type;
    MemCh *m;
    i32 idx;
    Handler *chain;
    Serve *ctx;
} Req;

typedef struct serve_req {
    Type type;
    MemCh *m;
    i32 idx;
    Handler *chain;
    Serve *ctx;
    /* begin network req */
    Cursor *cursor;
    Roebling *rbl;
    /* end network req */
} NetworkReq;
