typedef struct proto {
    Type type;
    util u;
    Buff *in;
    Span *outSpan;
    Buff *out;
    void *ctx;
} ProtoCtx;

ProtoCtx *ProtoCtx_Make(MemCh *m);
