enum http_methods {
    __TYPE_HTTP_START = _TYPE_HTTP_START,
    TYPE_METHOD_GET,
    TYPE_METHOD_SET,
    TYPE_METHOD_UPDATE, 
    TYPE_METHOD_DELETE,
    __TYPE_HTTP_END,
};

typedef struct proto {
    Type type;
    String *path;
    byte method;
    String *body;
    String *host;
    Virtual *session;
    /* mid parsing */
    i64 contentLength;
    String *shelf;
    Roebling *rbl;
    String *nextHeader;
} HttpProto;

ProtoDef *Http_ProtoDefMake(MemCtx *m, Serve *sctx);

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
