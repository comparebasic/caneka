typedef struct serve_req {
    Type type;
    MemCtx *m;
    Handler *handler;
    int queueIdx;
    int fd;
    String *id;
    Serve *sctx;
    Proto *proto;
    struct {
        String *shelf;
        Roebling *rbl;
    } in;
    struct {
        String *response;
        Cursor *cursor;
    } out;
} Req;

Req *Req_Make(MemCtx *m, struct serve_ctx *sctx, Proto *proto);
status Req_SetError(Serve *sctx, Req *req, String *msg);
status Req_Recv(Serve *sctx, Req *req);
status Req_Read(Serve *sctx, Req *req);
status Req_Respond(Serve *sctx, Req *req);
status Req_Handle(Serve *sctx, Req *req);
status Req_Process(Serve *sctx, Req *req);
status Req_SetResponse(Req *req, int code, Span *headers, String *body);
