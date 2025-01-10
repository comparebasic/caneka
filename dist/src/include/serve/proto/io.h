typedef struct io_proto {
    Type type;
    char *(*toLog)(Req *req);
    Span *headers_tbl;
    String *body;
    Lookup *methods;
} IoProto;

ProtoDef *IoProtoDef_Make(MemCtx *m, Maker reqMake);
Proto *IoProto_Make(MemCtx *m, ProtoDef *def);
