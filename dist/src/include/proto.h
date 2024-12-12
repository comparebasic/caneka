typedef struct proto {
    Type type;
    char *(*toLog)(struct serve_req *req);
    Span *headers_tbl;
    String *body;
    Lookup *methods;
} Proto;

typedef struct protodef {
    Type type;
    cls reqType;
    Maker req_mk;
    Maker proto_mk;
    Handler *(*getHandler)(struct serve_ctx *sctx, struct serve_req *req);
    Abstract *source;
    GetDelayFunc getDelay;
    Lookup *methods;
} ProtoDef;

ProtoDef *ProtoDef_Make(MemCtx *m, cls type,
    Maker req_mk,
    Maker proto_mk,
    Lookup *methods
);

char *Proto_ToChars(struct proto *proto);
