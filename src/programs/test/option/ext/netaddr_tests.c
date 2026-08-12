#include <external.h>
#include <caneka.h>
#include <test_module.h>


status NetAddr6_Tests(MemCh *m){
    Debug_Push(m, NULL);

    status r = READY;
    void *args[5];

    NetAddr *addr = NetAddr_Make6(m);

    NetAddr_SetFromStr6(m, addr, S(m, "ab129:01:124c:1234"));
    NetAddr_SetFromStr6(m, addr, S(m, "::1"));
    NetAddr_SetFromStr6(m, addr, S(m, "::"));
    NetAddr_SetFromStr6(m, addr, S(m, "0124::12"));
    NetAddr_SetFromStr6(m, addr, S(m, "0124::3:12"));

    
    Return(m, r);
}
