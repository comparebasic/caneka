#include <external.h>
#include <caneka.h>
#include <test_module.h>

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

    word nonce = 37;
    StrVec_SaltedDigest(m, v, salt, (digest *)hash->bytes, nonce);
    hex = Str_ToHex(m, hash);
    expected = Str_CstrRef(m,
        "b797cc5b4d1a08c9ab7ec358e6d714152bc51de1faa51a72d2fbb88c10d937b0");

    args[0] = expected;
    args[1] = hex;
    args[2] = NULL;
    r |= Test(Equals(hex, expected), 
        "Salted StrVec Sha256 matches, expected @, have @", args);

    Single *public = Ptr_Wrapped(m, NULL, ZERO);
    Single *secret = Ptr_Wrapped(m, NULL, ZERO);
    SignPair_Make(m, public, secret);

    Str *pubHex = Str_ToHex(m, (Str *)secret->val.ptr);

    Single *publicB = Ptr_Wrapped(m, NULL, ZERO);
    Single *secretB = Ptr_Wrapped(m, NULL, ZERO);
    SignPair_Make(m, publicB, secretB);

    r |= Test(Equals(publicB, public),
        "Public and public second generation are equals", NULL);
    r |= Test(Equals(secretB, secret),
        "Secret and secret second generation are equals", NULL);

    Str *message = Str_CstrRef(m, "I super-master person, do hereby sign some cool,"
        " super-fancy stuff, that totally needed to be signed, like yesterday (sorry)");
    Str *sig = SignPair_Sign(m, StrVec_From(m, message), secret);

    Str *sigHex = Str_ToHex(m, sig);

    status valid = SignPair_Verify(m, StrVec_From(m, hash), sig, public);

    args[0] = message;
    args[1] = sigHex;
    args[2] = NULL;
    r |= Test(valid & SUCCESS, "Message validates: @ -> sig:@", args);

    DebugStack_Pop();
    return r;
}
