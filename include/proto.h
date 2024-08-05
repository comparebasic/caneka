typedef struct proto {
    Type type;
    char *(*toLog)(struct serve_req *req);
    Span *headers_tbl;
    String *body;
} Proto;

typedef struct protodef {
    Type type;
    Span *parsers_pmk;
    Maker req_mk;
    Maker proto_mk;
    Lookup *handlers;
    Lookup *methods;
    Abstract *source;
    Span *hdrHandlers_tbl_mk;
} ProtoDef;

ProtoDef *ProtoDef_Make(MemCtx *m, cls type,
    Maker req_mk,
    Maker proto_mk,
    Abstract *source
);

char *Proto_ToChars(struct proto *proto);
