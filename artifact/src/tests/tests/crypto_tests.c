#include <external.h>
#include <caneka.h>

status CryptoStrings_Tests(MemCtx *gm){
    status r = READY;
    MemCtx *m = MemCtx_Make();
    String *s;
    String *sum;
    String *hex;

    s = Cont(m, bytes("Hidy Hody, Hi ho ho hi ho ho"));
    sum = Digest_Sha256(m, s);
    hex = String_ToHex(m, sum);
    r |= Test(String_EqualsBytes(hex, bytes("68760b4a88c113b8ce4aed641911caf8ef01d5969754a20271e601698997881b")), "Sha256 matches, have '%s'",hex->bytes);

    int length = 512;
    s = Crypto_RandomString(m, 512);
    int l = String_Length(s);
    r |= Test(l == 512, "Random string length is expected, have %d", l);

    MemCtx_Free(m);
    return r;
}
