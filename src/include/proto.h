typedef struct proto {
    Type type;
    char *(*toLog)(struct serve_req *req);
    Span *headers_tbl;
    String *body;
} Proto;

typedef struct protodef {
    Type type;
    cls reqType;
    Maker req_mk;
    Maker proto_mk;
    Lookup *handlers;
    Abstract *source;
} ProtoDef;

ProtoDef *ProtoDef_Make(MemCtx *m, cls type,
    Maker req_mk,
    Maker proto_mk,
    Lookup *handlers
);

char *Proto_ToChars(struct proto *proto);
