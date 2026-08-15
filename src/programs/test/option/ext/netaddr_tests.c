#include <external.h>
#include <caneka.h>
#include <test_module.h>


status NetAddr6_Tests(MemCh *m){
    Debug_Push(m, NULL);

    status r = READY;
    void *args[5];

    NetAddr *addr = NetAddr_Make6(m);

    Str *s = S(m, "ab10:01:12c2:1233:1004:2005:6:7000");
    NetAddr_SetFromStr6(m, addr, s);
    args[0] = s;
    args[1] = Ip6_ToStr(m, 
        Str_Ref(m, (byte *)&addr->net.ip6addr.sin6_addr, IP6_ALLOC, IP6_ALLOC, ZERO));
    args[2] = addr;
    args[3] = NULL;
    r |= Test(Equals(args[0], args[1]),
        "Ip6 address is properly set from string @ -> @ of @^0", args);

    s = S(m, "::1");
    NetAddr_SetFromStr6(m, addr, s);
    args[0] = s;
    args[1] = Ip6_ToStr(m, 
        Str_Ref(m, (byte *)&addr->net.ip6addr.sin6_addr, IP6_ALLOC, IP6_ALLOC, ZERO));
    args[2] = addr;
    args[3] = NULL;
    r |= Test(Equals(args[0], args[1]),
        "Ip6 address is properly set from string @ -> @ of @^0", args);

    s = S(m, "::");
    NetAddr_SetFromStr6(m, addr, s);
    args[0] = s;
    args[1] = Ip6_ToStr(m, 
        Str_Ref(m, (byte *)&addr->net.ip6addr.sin6_addr, IP6_ALLOC, IP6_ALLOC, ZERO));
    args[2] = addr;
    args[3] = NULL;
    r |= Test(Equals(args[0], args[1]),
        "Ip6 address is properly set from string @ -> @ of @^0", args);

    s = S(m, "0124::12");
    NetAddr_SetFromStr6(m, addr, s);
    args[0] = s;
    args[1] = Ip6_ToStr(m, 
        Str_Ref(m, (byte *)&addr->net.ip6addr.sin6_addr, IP6_ALLOC, IP6_ALLOC, ZERO));
    args[2] = addr;
    args[3] = NULL;
    r |= Test(Equals(args[0], args[1]), 
        "Ip6 address is properly set from string @ -> @ of @^0", args);

    s = S(m, "0124:1::3:12");
    NetAddr_SetFromStr6(m, addr, s);
    args[0] = s;
    args[1] = Ip6_ToStr(m, 
        Str_Ref(m, (byte *)&addr->net.ip6addr.sin6_addr, IP6_ALLOC, IP6_ALLOC, ZERO));
    args[2] = addr;
    args[3] = NULL;
    r |= Test(Equals(args[0], args[1]), 
        "Ip6 address is properly set from string @ -> @ of @^0", args);
    
    Return(m, r);
}
