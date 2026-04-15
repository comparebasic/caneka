#include <external.h>
#include <caneka.h>
#include <test_module.h>

status Encoding_Tests(MemCh *m){
    DebugStack_Push(NULL, 0);
    status r = READY;
    void *args[5];
    
    Str *s;
    Str *s2;
    Str *e;
    Str *expected;
    StrVec *v;
    StrVec *ev;
    StrVec *expectedV;

    s = Str_CstrRef(m, "Hidy Ho, there, here's a short one");
    e = Str_ToHex(m, s);
    expected = Str_CstrRef(m,
        "4869647920486f2c2074686572652c2068657265277320612073686f7274206f6e65");

    args[0] = expected;
    args[1] = e;
    args[2] = NULL;
    r |= Test(Equals(e, expected), 
        "Hex str is as expected &, have &", args);
    
    s2 = Str_FromHex(m, e);

    args[0] = s;
    args[1] = s2;
    args[2] = NULL;
    r |= Test(Equals(s2, s), 
        "From hex equals original str,  expected &, have &", args);

    DebugStack_Pop();
    return r;
}
