typedef struct sub_proto {
    Type type;
    char *(*toLog)(struct req *req);
    ProtoDef *def;
    Abstract *custom;
    /* end proto */
    String *name;
    ProcIoSet *procio;
} SubProto;

ProtoDef *SubProtoDef_Make(MemCtx *m, Maker reqMake);
Proto *SubProto_Make(MemCtx *m, ProtoDef *def, word flags);
