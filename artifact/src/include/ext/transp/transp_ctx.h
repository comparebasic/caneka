typedef i64 (*TranspFunc)(struct transp_ctx *ctx, word flags);
typedef struct transp_ctx {
    Type type;
    word status;
    word prevStatus;
    MemCh *m;
    Stream *sm;
    Iter it;
    Lookup *lk;
    TranspFunc func;
} TranspCtx;

TranspCtx *TranspCtx_Make(MemCh *m, Stream *sm, Lookup *lk);
