#include <external.h>
#include <caneka.h>
#include <tests.h>

status Crypto_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    status r = READY;
    MemCh *m = MemCh_Make();
    Abstract *args[5];

    Str *s = Str_CstrRef(m, "Yay test string!");
    Str *digest = Str_ToSha256(m, s);
    digest = Str_ToHex(m, digest);

    Str *expected = Str_CstrRef(m,
        "8a87db5fb3a1e4491a04644396437064375c00bdaf7fea33bd4645ba138539b5");

    r |= Test(Equals((Abstract *)digest, (Abstract *)expected), 
        "Sha256 Matches", NULL);

    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}
