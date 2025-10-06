#define TCP_POLL_DELAY 10
#define ACCEPT_AT_ONEC_MAX 192
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
    StepFunc func;
} TcpCtx;

TcpCtx *TcpCtx_Make(MemCh *m);
