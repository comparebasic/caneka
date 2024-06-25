typedef struct serve_req {
    MemCtx *m;
    status state;
    int fd;
    String *id;
    int direction;
    Serve *sctx;
    struct {
        String *path;
        byte method;
        String *body;
        /* mid parsing */
        int contentLength;
        String *host;
        Roebling *sexp;
        String *_shelf;
        String *nextHeader;
    } in;
    struct {
        String *response;
        SCursor *cursor;
    } out;
} Req;

Req *Req_Make(struct serve_ctx *sctx);
status Req_Parse(Serve *sctx, Req *req, String *s, ParserMaker parsers[]);
status Req_SetError(Serve *sctx, Req *req, String *msg);
status Req_Recv(Serve *sctx, Req *req);
status Req_Handle(Serve *sctx, Req *req);
status Req_Process(Serve *sctx, Req *req);

char *Method_ToString(int method);
