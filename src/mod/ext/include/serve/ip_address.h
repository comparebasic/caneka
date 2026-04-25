typedef struct ip_address {
    Type type;
    cls proto;
    cls method;
    i32 port;
    union {
        quad v4;
        quad v6[2];
    } ip;
} IpAddress;

cls Bytes_ToHttpProto(MemCh *m, void *a);
