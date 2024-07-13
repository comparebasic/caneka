typedef struct proto {
    Type type;
    char *(*toLog)(struct serve_req *req);
} Proto;

typedef struct protodef {
    Type type;
    Span *parsers_pmk;
    Maker req_mk;
    Maker proto_mk;
    Lookup *handlers;
    Lookup *methods;
    Abstract *virt;
} ProtoDef;

ProtoDef *ProtoDef_Make(MemCtx *m, cls type,
    Maker req_mk,
    Maker proto_mk,
    Span *parsers_pmk,
    Lookup *handlers,
    Lookup *methods,
    Abstract *virt
);

char *Proto_ToChars(struct proto *proto);
