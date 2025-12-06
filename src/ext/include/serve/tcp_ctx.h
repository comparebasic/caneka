typedef struct tcp_ctx {
    Type type;
    i32 port;
    Str *inet4;
    Str *inet6;
    TaskPopulate populate;
    StepFunc finalize; 
    SourceFunc defaultData;
    StrVec *path;
    Route *pages;
    Route *inc;
    NodeObj *nav;
    struct {
        struct timespec start;
        i64 open;
        i64 error;
        i64 served;
    } metrics;
} TcpCtx;

TcpCtx *TcpCtx_Make(MemCh *m);
