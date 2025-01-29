typedef struct io_proto {
    Type type;
    char *(*toLog)(struct req *req);
    ProtoDef *def;
    Span *headers_tbl;
    String *body;
    Lookup *methods;
    Abstract *data;
} IoProto;

ProtoDef *IoProtoDef_Make(MemCtx *m, Maker reqMake);
Proto *IoProto_Make(MemCtx *m, ProtoDef *def);
