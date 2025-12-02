#include <external.h>
#include <caneka.h>
#include <test_module.h>
#include "tests.h"

status Crypto_Tests(MemCh *m){
    DebugStack_Push(NULL, 0);
    status r = READY;
    void *args[5];

    Str *s = Str_CstrRef(m, "Yay test string!");
    Str *hash = Str_Make(m, DIGEST_SIZE);
    hash->length = DIGEST_SIZE;
    Str_ToSha256(m, s, (digest *)hash->bytes);
    Str *hex = Str_ToHex(m, hash);

    Str *expected = Str_CstrRef(m,
        "8a87db5fb3a1e4491a04644396437064375c00bdaf7fea33bd4645ba138539b5");

    args[0] = s;
    args[1] = expected;
    args[2] = hex;
    args[3] = NULL;
    r |= Test(Equals(hex, expected), 
        "Str Sha256 Matches, expected @ -> $, have $", args);

    StrVec *v = StrVec_Make(m);
    StrVec_Add(v, Str_CstrRef(m, "Yay "));
    StrVec_Add(v, Str_CstrRef(m, "test string!"));

    r |= Test(Equals(s, v), 
        "StrVec and Str text are identical: & vs &", args);

    StrVec_ToSha256(m, v, (digest *)hash->bytes);
    hex = Str_ToHex(m, hash);

    args[0] = s;
    args[1] = hex;
    args[2] = NULL;
    r |= Test(Equals(hex, expected), 
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

    Str_ToSha256(m, longStr, (digest *)hash->bytes);
    hex = Str_ToHex(m, hash);
    expected = Str_CstrRef(m,
        "da43c4b3c5e3667dcaf17f184880241908c2ab1f9e8d2f5bbf62825df6303ab6");

    args[0] = expected;
    args[1] = hex;
    args[2] = NULL;
    r |= Test(Equals(hex, expected), 
        "longstr Str Sha256 matches, expected @, have @", args);

    StrVec_ToSha256(m, v, (digest *)hash->bytes);
    hex = Str_ToHex(m, hash);
    expected = Str_CstrRef(m,
        "da43c4b3c5e3667dcaf17f184880241908c2ab1f9e8d2f5bbf62825df6303ab6");

    args[0] = expected;
    args[1] = hex;
    args[2] = NULL;
    r |= Test(Equals(hex, expected), 
        "StrVec Sha256 matches, expected @, have @", args);

    v = StrVec_Make(m);
    StrVec_Add(v, Str_CstrRef(m, "One "));
    StrVec_Add(v, Str_CstrRef(m, "Two "));
    StrVec_Add(v, Str_CstrRef(m, "Three "));
    StrVec_Add(v, Str_CstrRef(m, "Four "));
    Str *salt = Str_CstrRef(m, "French fries, and potato chips, and curry, "
    "and garlic break, and oceans, and taffy, and lots o things have salt!");

    StrVec_SaltedDigest(m, v, salt, (digest *)hash->bytes);
    hex = Str_ToHex(m, hash);
    expected = Str_CstrRef(m,
        "466b336b19f46a9684b4515f43ac4b3a06e080369f99394434e4b8d4f4cbc9de");

    args[0] = expected;
    args[1] = hex;
    args[2] = NULL;
    r |= Test(Equals(hex, expected), 
        "Salted StrVec Sha256 matches, expected @, have @", args);

    StrVec *phrase = StrVec_From(m, Str_CstrRef(m, "My favorite book is the one "
        "about a dog and a cat and a bannana, and a car."));

    Str *public = Str_DigestAlloc(m);
    Str *secret = Str_DigestAlloc(m);
    SignPair_Make(m, public, secret, phrase);

    Str *pubHex = Str_ToHex(m, public);
    Str *secretHex = Str_ToHex(m, secret);

    Str *publicB = Str_DigestAlloc(m);
    Str *secretB = Str_DigestAlloc(m);
    SignPair_Make(m, publicB, secretB, phrase);

    r |= Test(Equals(publicB, public),
        "Public and public second generation are equals", NULL);
    r |= Test(Equals(secretB, secret),
        "Secret and secret second generation are equals", NULL);

    Str *message = Str_CstrRef(m, "I super-master person, do hereby sign some cool,"
        " super-fancy stuff, that totally needed to be signed, like yesterday (sorry)");
    Str_ToSha256(m, message, (digest *)hash->bytes);
    Str *sig = SignPair_Sign(m, hash, secret);

    Str *sigHex = Str_ToHex(m, sig);

    status valid = SignPair_Verify(m, hash, sig, public);

    args[0] = message;
    args[1] = sigHex;
    args[2] = NULL;
    r |= Test(valid & SUCCESS, "Message validates: @ -> sig:@", args);

    DebugStack_Pop();
    return r;
}
