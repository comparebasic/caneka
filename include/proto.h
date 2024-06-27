typedef struct proto {
    Type type;
    char *(*toLog)(struct serve_req *req);
} Proto;

typedef struct serve_req *(*MakeReq)(MemCtx *m, struct serve_ctx *sctx);

typedef struct protodef {
    Type type;
    Span *parsers;
    MakeReq req;
    Lookup *handlers;
    Lookup *methods;
    Abstract *virt;
} ProtoDef;

ProtoDef *ProtoDef_Make(MemCtx *m, Lookup *handlers, Lookup *methods, Abstract *virt);
char *Proto_ToChars(struct proto *proto);
