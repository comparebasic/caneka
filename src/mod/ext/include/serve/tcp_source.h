typedef struct tcp_source {
    Type type;
    i32 new_fd;
    HostEnt *clientEnt;
} TcpSource;

TcpSource *TcpSource_Make(MemCh *m);
