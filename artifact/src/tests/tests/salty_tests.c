#include <external.h>
#include <caneka.h>

char *cstr = ""
    "They system scales in powers of the span size. in this case dimension one "
    "has 8 slots, dimension 2 has 64 slots and dimension 3 has 512 slots. The "
    "standard Caneka span has a dimsize of 16 with 16/256/4096/65,536/1,048,576 "
    "per dimension. This makes a 5 dimension table capable of holding around a "
    "million disperate values.\n"
    "\n"
    "In the below example, the disparate set of values from 0,1,2,3,6, and,225 "
    "can all be consistently accessed and stored with only 5 allocated span "
    "slabs."
    ;

char *cstrTwo = ""
    "One factor of consideration is that each dimension adds an additional hop "
    "to element accesss. This is not fundementally different than an expanding "
    "binary tree where comparisons are common no matter how consisten they are. "
    "However, because of this, the Span shines in use-cases where items are "
    "accessed either both directly and iteratively becuase iteration can "
    "traverse each span with no lookup cost between consecutive items."
    ;

status Salty_Tests(MemCtx *gm){
    DebugStack_Push(NULL, 0);
    status r = READY;
    MemCtx *m = MemCtx_Make();


    String *keyOne = String_Make(m, bytes(cstr));
    String *contentTwo = String_Make(m, bytes(cstrTwo));
    String *enc = Salty_Enc(m, keyOne, contentTwo);
    r |= Test(enc != NULL && enc->type.state & FLAG_STRING_BINARY,
        "enc is not null and binary flagged");
    String *dec = Salty_Dec(m, keyOne, enc);
    r |= Test(dec != NULL && (dec->type.state & FLAG_STRING_BINARY) == 0,
        "enc is not null and not binary flagged");
    r |= Test(String_Equals(contentTwo, dec),
        "decrypted equals original");

    String *phrase = NULL;
    String *key = NULL;
    EncPair *pair = NULL;
    EncPair *result = NULL;
    String *s = NULL;

    Access *ac = Access_Make(m, Cont(m, bytes("test")), NULL);
    Access *system = Access_Make(m, Cont(m, bytes("system")), NULL);

    status keysAdded = Tests_AddTestKey(gm);

    r |= Test((keysAdded & SUCCESS) != 0, "Test key added to key chain");

    key = EncPair_GetKey(m, Cont(m, bytes("bogus")), ac);
    r |= Test(key == NULL, "'bogus' user is unable to access 'test' key");
    key = EncPair_GetKey(m, Cont(m, bytes("test")), ac);
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
    DebugStack_Pop();
    return r;
}

