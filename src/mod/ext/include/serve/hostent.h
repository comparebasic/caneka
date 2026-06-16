typedef struct host_ent_wr {
    Type type;
    struct hostent *ent;
    Str *name;
    Span *addrs;
} HostEnt;

HostEnt *HostEnt_Make(MemCh *m);
HostEnt *HostEnt_FromName(MemCh *m, Str *s);
quad *HostEnt_AddrIp4(HostEnt *h);
util *HostEnt_AddrIp6(HostEnt *h);
