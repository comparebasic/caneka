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

#define IP4_STR_ALLOC 18
#define IP6_ALLOC 16
#define IP6_STR_ALLOC 39
Str *Ip4_ToStr(MemCh *m, quad ip4);
Str *Ip6_ToStr(MemCh *m, Str *ref);
quad Str_ToIp4(MemCh *m, Str *s);
quad Quad_ToIp4(byte a, byte b, byte c, byte d);
