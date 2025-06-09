typedef i64 (*TranspFunc)(struct transp_ctx *ctx);
typedef struct transp_ctx {
    Type type;
    word status;
    word prevStatus;
    MemCh *m;
    Stream *sm;
    Iter it;
    Lookup *lk;
} TranspCtx;

TranspCtx *TranspCtx_Make(MemCh *m, Stream *sm, Lookup *lk);
i64 Transp(Transp *tp);
