typedef struct protodef {
    Type type;
    cls reqType;
    Maker req_mk;
    Maker proto_mk;
    struct handler *(*getHandler)(struct serve_ctx *sctx, struct req *req);
    Abstract *source;
    GetDelayFunc getDelay;
    Abstract *custom;
} ProtoDef;

typedef struct proto {
    Type type;
    char *(*toLog)(struct req *req);
    ProtoDef *def;
} Proto;

ProtoDef *ProtoDef_Make(MemCtx *m, cls type,
    Maker req_mk,
    Maker proto_mk
);

char *Proto_ToChars(struct proto *proto);
