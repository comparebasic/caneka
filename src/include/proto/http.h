typedef struct http_proto {
    Type type;
    char *(*toLog)(Req *req);
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

ProtoDef *HttpProtoDef_Make(MemCtx *m);
Proto *HttpProto_Make(MemCtx *m, ProtoDef *def);
Req *HttpReq_Make(MemCtx *m, Serve *sctx);
