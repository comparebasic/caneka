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

    Access *ac = Access_Make(m, Cont(m, bytes("test")), NULL);
    Access *system = Access_Make(m, Cont(m, bytes("system")), NULL);

    status keysAdded = Tests_AddTestKey(gm);

    r |= Test((keysAdded & SUCCESS) != 0, "Test key added to key chain");

    key = EncPair_GetKey(Cont(m, bytes("bogus")), ac);
    r |= Test(key == NULL, "'bogus' user is unable to access 'test' key");
    key = EncPair_GetKey(Cont(m, bytes("test")), ac);
    r |= Test(key != NULL, "'test' user can access 'test' key, have, '%s'", key->bytes);

    s = Cont(m, bytes("One fell over the nest of another, things are good, and that's a never ending scenario, of life!"));

    pair = EncPair_Make(m, Cont(m, bytes("test")), NULL, s,  ac, s->length);
    EncPair_Fill(m, pair, ac);
    result = EncPair_Make(m, Cont(m, bytes("test")), pair->enc, NULL, ac, s->length);

    r |= Test(String_Equals(pair->enc, result->enc), "Encoded matches from original to result");
    EncPair_Fill(m, result, ac);
    r |= Test(String_Equals(pair->dec, result->dec), "Decoded matches from original to result");

    char *cstr = "qerjfkdlkieurjfhdic3k83";
    String *salt = String_Make(m, bytes(cstr));;
    pair = EncPair_Make(m, Cont(m, bytes("test")), NULL, salt,  ac, salt->length);
    EncPair_Fill(m, pair, ac);
    result = EncPair_Make(m, Cont(m, bytes("test")), pair->enc, NULL, ac, salt->length);
    EncPair_Fill(m, result, ac);
    r |= Test(String_Equals(pair->dec, result->dec), "Second decoded matches from original to result");

    pair = EncPair_Make(m, String_Make(m, bytes("test")), NULL, String_Make(m, bytes("zpants!1923")), ac, strlen("zpants!1923"));
    EncPair_Fill(m, pair, ac);
    EncPair_Conceal(m, pair);

    char *exp ="enc/3={key:s/4=test;enc:s/32=2bfc98b22fb8b02d33ffe258111e3438;length:i64/2=11;}";
    
    String *os = String_ToEscaped(m, Oset_To(m, NULL, (Abstract *)pair));
    r |= Test(String_EqualsBytes(os, bytes(exp)), "EncPair to oset string is equal, expected '%s', have '%s'", exp, os->bytes);

    MemCtx_Free(m);

    r |= SUCCESS;
    return r;
}

