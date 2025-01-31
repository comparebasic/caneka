typedef struct http_proto {
    Type type;
    char *(*toLog)(struct req *req);
    Span *headers_tbl;
    String *body;
    Lookup *methods;
    /* end proto props */
    String *path;
    String *sid;
    byte method;
    String *host;
    Abstract *session;
    /* mid parsing */
    i64 contentLength;
    String *nextHeader;
    ProtoDef *def;
} HttpProto;

struct protodef *HttpProtoDef_Make(MemCtx *m);
struct proto *HttpProto_Make(MemCtx *m, struct protodef *def, word flags);
struct req *HttpReq_Make(MemCtx *m, struct serve_ctx *sctx, word flags);
