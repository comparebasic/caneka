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

    args[0] = (Abstract *)s;
    args[1] = (Abstract *)digest;
    args[2] = NULL;
    r |= Test(Equals((Abstract *)digest, (Abstract *)expected), 
        "Str Sha256 Matches @ -> $", args);

    StrVec *v = StrVec_Make(m);
    StrVec_Add(v, Str_CstrRef(m, "Yay "));
    StrVec_Add(v, Str_CstrRef(m, "test string!"));

    r |= Test(Equals((Abstract *)s, (Abstract *)v), 
        "StrVec and Str text are identical: & vs &", args);

    digest = StrVec_ToSha256(m, v);
    digest = Str_ToHex(m, digest);

    args[0] = (Abstract *)s;
    args[1] = (Abstract *)digest;
    args[2] = NULL;
    r |= Test(Equals((Abstract *)digest, (Abstract *)expected), 
        "StrVec Sha256 Matches & -> @", args);

    StrVec_Add(v, Str_CstrRef(m, "And now we add a whole bunch of random stuff"
        ". To make it test what its like to have multiple cycle blocks."
        "And pray to the comp-sci god and goddess that our bits align."
        "Becuase we all know that its getting hard out there for nerds."));

    StrVec_Add(v, Str_CstrRef(m, "And... Wow 512 is a lot of text. "
        ". Gonna write more more more more more more more more one."
        ". Gonna write more more more more more more more more two."
        ". Gonna write more more more more more more more more three."
        ". Gonna write more more more more more more more more four."
        ". Gonna write more more more more more more more more five."
        ". Gonna write more more more more more more more more six."
        ". Gonna write more more more more more more more more seven."
        ". Gonna write more more more more more more more more eight."
        ". Gonna write more more more more more more more more nine."
        ". Gonna write more more more more more more more more ten."));


    Str *longStr = StrVec_Str(m, v);

    digest = Str_ToSha256(m, longStr);
    digest = Str_ToHex(m, digest);
    expected = Str_CstrRef(m,
        "da43c4b3c5e3667dcaf17f184880241908c2ab1f9e8d2f5bbf62825df6303ab6");

    args[0] = (Abstract *)expected;
    args[1] = (Abstract *)digest;
    args[2] = NULL;
    r |= Test(Equals((Abstract *)digest, (Abstract *)expected), 
        "longstr Str Sha256 matches, expected @, have @", args);

    digest = StrVec_ToSha256(m, v);
    digest = Str_ToHex(m, digest);
    expected = Str_CstrRef(m,
        "da43c4b3c5e3667dcaf17f184880241908c2ab1f9e8d2f5bbf62825df6303ab6");

    args[0] = (Abstract *)expected;
    args[1] = (Abstract *)digest;
    args[2] = NULL;
    r |= Test(Equals((Abstract *)digest, (Abstract *)expected), 
        "StrVec Sha256 Matches expected @, have @", args);

    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}
