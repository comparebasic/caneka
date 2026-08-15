#include <external.h>
#include <caneka.h>
#include <test_module.h>


status NetAddr6_Tests(MemCh *m){
    Debug_Push(m, NULL);

    status r = READY;
    void *args[5];

    NetAddr *addr = NetAddr_Make6(m);

    Str *s = S(m, "ab129:01:124c:1234");
    NetAddr_SetFromStr6(m, addr, s);
    args[0] = s;
    args[1] = addr;
    args[2] = NULL;
    Out("@ -> @^0\n", args);

    s = S(m, "::1");
    NetAddr_SetFromStr6(m, addr, s);
    args[0] = s;
    args[1] = addr;
    args[2] = NULL;
    Out("@ -> @^0\n", args);

    s = S(m, "::");
    NetAddr_SetFromStr6(m, addr, s);
    args[0] = s;
    args[1] = addr;
    args[2] = NULL;
    Out("@ -> @^0\n", args);

    s = S(m, "0124::12");
    NetAddr_SetFromStr6(m, addr, s);
    args[0] = s;
    args[1] = addr;
    args[2] = NULL;
    Out("@ -> @^0\n", args);

    s = S(m, "0124::3:12");
    NetAddr_SetFromStr6(m, addr, s);
    args[0] = s;
    args[1] = addr;
    args[2] = NULL;
    Out("@ -> @^0\n", args);

    r |= ERROR;
    
    Return(m, r);
}
