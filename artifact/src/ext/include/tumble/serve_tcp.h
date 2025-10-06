typedef struct tcp_ctx {
    Type type;
    i32 port;
    struct {
        microTime start;
        i32 open;
        i32 error;
        i32 served;
    } metrics;
    SourceMakerFunc mk;
} TcpCtx;
