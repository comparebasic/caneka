typedef struct netaddr {
    Type type;
    union { 
        struct sockaddr_in ip4addr;
        struct sockaddr_in6 ip6addr;
    } net;
} NetAddr;

NetAddr *NetAddr_Make4(MemCh *m);
NetAddr *NetAddr_Make6(MemCh *m);
