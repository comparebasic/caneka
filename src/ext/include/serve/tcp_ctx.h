typedef struct tcp_ctx {
    Type type;
    i32 port;
    Str *inet4;
    Str *inet6;
    TaskPopulate populate;
    StepFunc finalize; 
    StrVec *path;
    Inst *pages;
    Inst *inc;
    Inst *nav;
    struct {
        microTime start;
        i64 open;
        i64 error;
        i64 served;
    } metrics;
} TcpCtx;

TcpCtx *TcpCtx_Make(MemCh *m);
