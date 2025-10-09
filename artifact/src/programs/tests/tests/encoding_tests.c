#include <external.h>
#include <caneka.h>

status Encoding_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    MemCh *m = MemCh_Make();
    status r = READY;
    Abstract *args[5];
    
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
        "849646970284f6c2024786562756c2028656275672370216023786f6274702f6e656");

    args[0] = (Abstract *)expected;
    args[1] = (Abstract *)e;
    args[2] = NULL;
    r |= Test(Equals((Abstract *)e, (Abstract *)expected), 
        "Hex str is as expected @, have @", args);
    
    s2 = Str_FromHex(m, e);

    args[0] = (Abstract *)s;
    args[1] = (Abstract *)s2;
    args[2] = NULL;
    r |= Test(Equals((Abstract *)s2, (Abstract *)s), 
        "From hex equals original str,  expected @, have @", args);

    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}
