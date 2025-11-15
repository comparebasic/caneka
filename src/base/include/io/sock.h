#define IP4_STR_ALLOC 18
status Sock_InetConnect(Buff *bf, quad ip4, util *ip6, i32 port);
Str *Ip4_ToStr(MemCh *m, quad ip4);
quad Str_ToIp4(MemCh *m, Str *s);
quad Quad_ToIp4(byte a, byte b, byte c, byte d);
