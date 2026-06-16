typedef struct host_ent_wr {
    Type type;
    struct hostent *ent;
    Span *addrs;
} HostEnt;

HostEnt *HostEnt_Make(MemCh *m);
HostEnt *HostEnt_FromName(MemCh *m, Str *s);
