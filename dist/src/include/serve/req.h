enum req_flags {
    REQ_ALPHA = 1 << 8,
    REQ_BRAVO = 1 << 9,
    REQ_CHARLIE = 1 << 10,
    REQ_DELTA = 1 << 11,
    REQ_ECHO = 1 << 12,
    REQ_FOXTROT = 1 << 13,
    REQ_GOLF = 1 << 14,
    REQ_HOTEL = 1 << 15,
};

typedef struct req {
    Type type;
    MemCtx *m;
    struct handler *handler;
    int queueIdx;
    int fd;
    String *id;
    struct serve_ctx *sctx;
    struct proto *proto;
    struct {
        String *shelf;
        struct roebling *rbl;
    } in;
    struct {
        String *response;
        Cursor *cursor;
    } out;
} Req;

struct req *Req_Make(MemCtx *m, struct serve_ctx *sctx, struct proto *proto);
status Req_SetError(struct serve_ctx *sctx, struct req *req, String *msg);
status Req_Recv(struct serve_ctx *sctx, struct req *req);
status Req_Read(struct serve_ctx *sctx, struct req *req);
status Req_Respond(struct serve_ctx *sctx, struct req *req);
status Req_Handle(struct serve_ctx *sctx, struct req *req);
status Req_Process(struct serve_ctx *sctx, struct req *req);
status Req_SetResponse(struct req *req, int code, Span *headers, String *body);
