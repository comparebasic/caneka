typedef struct host_ent_wr {
    Type type;
    i32 port;
    Str *name;
    Span *addrs;
    struct addrinfo *info;
} HostEnt;

HostEnt *HostEnt_Make(MemCh *m);
HostEnt *HostEnt_FromName(MemCh *m, Str *name, Str *service);
quad *HostEnt_AddrIp4(HostEnt *h);
util *HostEnt_AddrIp6(HostEnt *h);
