typedef struct proto {
    Type type;
    char *(*toLog)(struct req *req);
    Span *headers_tbl;
    String *body;
    Lookup *methods;
} Proto;

typedef struct protodef {
    Type type;
    cls reqType;
    Maker req_mk;
    Maker proto_mk;
    struct handler *(*getHandler)(struct serve_ctx *sctx, struct req *req);
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
