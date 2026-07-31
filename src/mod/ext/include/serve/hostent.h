typedef struct host_ent_wr {
    Type type;
    MemCh *m;
    Type objType;
    Str *name;
    Abstract *addr;
    struct addrinfo *info;
    struct crypto_tls_ctx *ctx;
    struct pollfd *pfd;
} HostEnt;

HostEnt *HostEnt_Make(MemCh *m);
HostEnt *HostEnt_FromName(MemCh *m, Str *name, Str *service);
quad *HostEnt_AddrIp4(HostEnt *h);
util *HostEnt_AddrIp6(HostEnt *h);
util HostEnt_Hash(void *a);
boolean HostEnt_Equals(void *_a, void *_b);
void HostEnt_OpenTcp(HostEnt *ent);
