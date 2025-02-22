#include <external.h>
#include <caneka.h>

char *cstrLong = ""
    "This family of product is design to audit and visualize Large-Langauge-Models "
    "and other Algorithmic based products. For the purpose of validating the safety, "
    "accuracy, and source of insights provided by these systems. This Builds on the "
    "runtime analysis expertise from the Caneka editor, which will span all popular "
    "languages by this time. The monetization strategy is expected to be a "
    "partnership with insurance companies in the business space for a reduced "
    "premium in exchange for greater certainty of safety."
    ;

status CryptoStrings_Tests(MemCtx *gm){
    DebugStack_Push(NULL, 0);
    status r = READY;
    MemCtx *m = MemCtx_Make();
    String *s;
    String *sum;
    String *hex;

    s = Cont(m, bytes("Hidy Hody, Hi ho ho hi ho ho"));
    sum = Digest_Sha256(m, s);
    r |= Test(sum != NULL, "Sum is not null");
    hex = String_ToHex(m, sum);
    r |= Test(String_EqualsBytes(hex, bytes("68760b4a88c113b8ce4aed641911caf8ef01d5969754a20271e601698997881b")), "Sha256 matches, have '%s'", hex->bytes);

    int length = 512;
    s = Crypto_RandomString(m, 512);
    int l = String_Length(s);
    r |= Test(l == 512, "Random string length is expected, have %d", l);

    s = Cont(m, bytes(cstrLong));
    sum = Digest_Sha256(m, s);
    r |= Test(sum != NULL, "Sum is not null");
    hex = String_ToHex(m, sum);
    r |= Test(String_EqualsBytes(hex, bytes("368f2e30f6000781d2cc8397db2e835f5247a2f36fa167fece30d4c42d8bdd4c")), "Sha256 matches, have '%s'", hex->bytes);


    MemCtx_Free(m);
    DebugStack_Pop();
    return r;
}
