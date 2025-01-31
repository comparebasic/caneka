typedef struct io_proto {
    Type type;
    char *(*toLog)(struct req *req);
    ProtoDef *def;
    Abstract *source;
    Abstract *custom;
} IoProto;

ProtoDef *IoProtoDef_Make(MemCtx *m, Maker reqMake);
Proto *IoProto_Make(MemCtx *m, ProtoDef *def, word flags);
