typedef struct sub_proto {
    Type type;
    char *(*toLog)(struct req *req);
    ProtoDef *def;
    /* end proto */
    Span *cmds_p;
    String *name;
    int code;
    Abstract *super;
} SubProto;

ProtoDef *SubProtoDef_Make(MemCtx *m, Maker reqMake);
Proto *SubProto_Make(MemCtx *m, ProtoDef *def, word flags);
