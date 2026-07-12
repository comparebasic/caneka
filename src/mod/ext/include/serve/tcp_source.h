typedef struct tcp_source {
    Type type;
    i32 new_fd;
    NetAddr *addr;
} TcpSource;

TcpSource *TcpSource_Make(MemCh *m);
