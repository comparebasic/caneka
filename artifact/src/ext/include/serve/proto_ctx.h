typedef struct proto {
    Type type;
    util u;
    Buff *in;
    Buff *out;
    Abstract *data;
} ProtoCtx;

ProtoCtx *ProtoCtx_Make(MemCh *m);
