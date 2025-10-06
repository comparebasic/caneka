typedef struct tcp_ctx {
    Type type;
    i32 port;
    Str *inet4;
    Str *inet6;
    struct {
        microTime start;
        i64 open;
        i64 error;
        i64 served;
    } metrics;
    SourceMakerFunc mk;
} TcpCtx;

TcpCtx *TcpCtx_Make(MemCh *m);
