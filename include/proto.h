enum proto_types {
    _TYPE_PROTO_START = _TYPE_CORE_END,
    TYPE_HTTP_PROTO,
    _TYPE_PROTO_END,
    _TYPE_HTTP_START,
    _TYPE_HTTP_END = __TYPE_HTTP_END,
};

typedef proto {
    Type type;
    Virtual in;
} Proto;

typedef protodef {
    Type type;
    Roebling *parser;
    Maker req;
    RlbLookup *handlers;
    RlbLookup *methods;
    Virtual *virt;
} ProtoDef;

ProtoDef ProtoDef_Make(MemCtx *m, RlbLookup *handlers, RlbLookup *methods, Virtual *virt);
