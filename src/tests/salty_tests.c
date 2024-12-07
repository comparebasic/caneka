#include <external.h>
#include <caneka.h>

status Salty_Tests(MemCtx *gm){
    status r = READY;
    MemCtx *m = MemCtx_Make();

    String *phrase = NULL;
    String *key = NULL;
    EncPair *pair = NULL;
    EncPair *result = NULL;
    String *s = NULL;

    r |= Test(CB_Phrase(m) != NULL, "Phrase is not null");

    Access *ac = Access_Make(m, Cont(m, bytes("test")), NULL);
    Access *system = Access_Make(m, Cont(m, bytes("system")), NULL);

    status keysAdded = Tests_AddTestKey(gm);

    r |= Test((keysAdded & SUCCESS) != 0, "Test key added to key chain");

    key = EncPair_GetKey(Cont(m, bytes("bogus")), ac);
    r |= Test(key == NULL, "'bogus' user is unable to access 'test' key");
    key = EncPair_GetKey(Cont(m, bytes("test")), ac);
    r |= Test(key != NULL, "'test' user can access 'test' key, have, '%s'", key->bytes);

    s = Cont(m, bytes("One fell over the nest of another, things are good, and that's a never ending scenario, of life!"));

    pair = EncPair_Make(m, Cont(m, bytes("test")), NULL, s,  ac);
    EncPair_Fill(m, pair, ac);
    result = EncPair_Make(m, Cont(m, bytes("test")), pair->enc, NULL, ac);

    r |= Test(String_Equals(pair->enc, result->enc), "Encoded matches from original to result");
    EncPair_Fill(m, result, ac);
    r |= Test(String_Equals(pair->dec, result->dec), "Decoded matches from original to result");


    pair = EncPair_Make(m, String_Make(m, bytes("test")), NULL, String_Make(m, bytes("zpants!1923")), ac);
    EncPair_Fill(m, pair, ac);
    EncPair_Conceal(m, pair);

    String *os = Oset_To(m, NULL, (Abstract *)pair);
    r |= Test(String_EqualsBytes(os, bytes("enc/3={key:s/4=test;enc:s/22=2bfc98b22fb8b02d33ffe2;dec:x/0=;}")), "Span to oset string is equal, have '%s'", os->bytes);

    MemCtx_Free(m);

    r |= SUCCESS;
    return r;
}

