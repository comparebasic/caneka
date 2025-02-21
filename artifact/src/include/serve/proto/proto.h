typedef struct protodef {
    Type type;
    cls reqType;
    FlagMaker req_mk;
    FlagMaker reqFree_mk;
    FlagMaker proto_mk;
    struct handler *(*getHandler)(struct serve_ctx *sctx, struct req *req);
    Abstract *source;
    GetDelayFunc getDelay;
    Abstract *custom;
    /* end protodef */
    Lookup *methods;
} ProtoDef;

typedef struct proto {
    Type type;
    char *(*toLog)(struct req *req);
    ProtoDef *def;
    Abstract *custom;
} Proto;

ProtoDef *ProtoDef_Make(MemCtx *m, cls type,
    FlagMaker req_mk,
    FlagMaker reqFree_mk,
    FlagMaker proto_mk
);

char *Proto_ToChars(struct proto *proto);
