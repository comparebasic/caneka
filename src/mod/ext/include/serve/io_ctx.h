typedef struct io_ctx {
    Type type;
    union {
        IpAddress ip;
        StrVec *path;
    } address;
    Req_Mk mk;
    DoFunc handle;
    DoFunc finalize;
    void *source;
} IoCtx;

IoCtx *IoCtx_Make(MemCh *m);
