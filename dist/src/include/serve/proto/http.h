typedef struct http_proto {
    Type type;
    char *(*toLog)(struct req *req);
    ProtoDef *def;
    Abstract *custom;
    /* end proto props */
    Span *headers_tbl;
    String *body;
    String *path;
    String *sid;
    byte method;
    String *host;
    /* mid parsing */
    i64 contentLength;
    String *nextHeader;
} HttpProto;

struct protodef *HttpProtoDef_Make(MemCtx *m);
struct proto *HttpProto_Make(MemCtx *m, struct protodef *def, word flags);
struct req *HttpReq_Make(MemCtx *m, struct serve_ctx *sctx, word flags);
