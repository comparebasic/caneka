typedef struct proto {
    Type type;
    util u;
    Cursor *in;
    Cursor *out;
    Abstract *data;
} ProtoCtx;

ProtoCtx *ProtoCtx_Make(MemCh *m);
