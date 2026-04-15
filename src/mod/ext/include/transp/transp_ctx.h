typedef i64 (*TranspFunc)(struct transp_ctx *ctx, word flags);
typedef struct transp_ctx {
    Type type;
    word status;
    i16 stackIdx;
    MemCh *m;
    Buff *bf;
    Iter it;
    Lookup *lk;
    TranspFunc func;
    Roebling *htmlEntRbl;
} TranspCtx;

TranspCtx *TranspCtx_Make(MemCh *m, Buff *bf, Lookup *lk);
