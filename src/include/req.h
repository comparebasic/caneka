typedef struct serve_req {
    Type type;
    MemCtx *m;
    Handler *handler;
    int fd;
    String *id;
    int direction;
    Serve *sctx;
    Proto *proto;
    struct {
        String *shelf;
        Roebling *rbl;
    } in;
    struct {
        String *response;
        SCursor *cursor;
    } out;
} Req;

Req *Req_Make(MemCtx *m, struct serve_ctx *sctx, Proto *proto, int direction);
status Req_SetError(Serve *sctx, Req *req, String *msg);
status Req_Recv(Serve *sctx, Req *req);
status Req_Handle(Serve *sctx, Req *req);
status Req_Process(Serve *sctx, Req *req);
