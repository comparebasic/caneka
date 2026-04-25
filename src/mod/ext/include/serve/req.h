typedef struct req {
    Type type;
    MemCh *m;
    i32 idx;
    util u;
} Req;

typedef Req *(*Req_Mk)(struct io_ctx *ctx);
