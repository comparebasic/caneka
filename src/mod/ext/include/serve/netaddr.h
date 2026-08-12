typedef struct netaddr {
    Type type;
    union { 
        struct sockaddr_in ip4addr;
        struct sockaddr_in6 ip6addr;
    } net;
} NetAddr;

void NetAddr_SetFromStr4(MemCh *m, NetAddr *net, Str *s);
void NetAddr_SetFromStr6(MemCh *m, NetAddr *net, Str *s);
void NetAddr_SetPort(MemCh *m, NetAddr *net, i32 port);
NetAddr *NetAddr_Make4(MemCh *m);
NetAddr *NetAddr_Make6(MemCh *m);
