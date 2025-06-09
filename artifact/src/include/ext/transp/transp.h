enum TRANSP_FLAGS {
    TRANSP_OPEN = 1 << 8,
    TRANSP_CLOSE = 1 << 9,
    TRANSP_BODY = 1 << 10,
};

i64 Transp(TranspCtx *ctx);
i64 Transp_Init(MemCh *m);
