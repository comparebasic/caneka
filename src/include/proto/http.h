typedef struct http_proto {
    Type type;
    char *(*toLog)(Req *req);
    Span *headers_tbl;
    String *body;
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

status HttpProto_Init(MemCtx *m);

ProtoDef *HttpProtoDef_Make(MemCtx *m);
Proto *HttpProto_Make(MemCtx *m, Serve *sctx);
Req *HttpReq_Make(MemCtx *m, Serve *sctx);

extern PatCharDef HttpV_RangeDef[];
extern PatCharDef EndNl_RangeDef[];
/* specific parsers */
Parser *Parser_Method(Roebling *sexp);
Parser *Parser_Space(Roebling *sexp);
Parser *Parser_Path(Roebling *sexp);
Parser *Parser_HttpV(Roebling *sexp);
Parser *Parser_EndNl(Roebling *sexp);
Parser *Parser_HColon(Roebling *sexp);
Parser *Parser_HEndAllNl(Roebling *sexp);
Parser *Parser_HEndNl(Roebling *sexp);
Span *HttpParser_Make(MemCtx *m, ProtoDef *def);

status HttpProtoDebug_Init(MemCtx *m);
