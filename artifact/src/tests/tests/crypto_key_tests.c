#include <external.h>
#include <caneka.h>

status Crypto_KeyTests(MemCtx *gm){
    DebugStack_Push("Crypto_KeyTests", TYPE_CSTR);
    status r = READY;
    MemCtx *m = MemCtx_Make();

    char buff[PATH_BUFFLEN];
    String *path = String_Init(m, STRING_EXTEND);
    char *cstr = "./artifact/fixtures/";
    String_AddBytes(m, path, bytes(cstr), strlen(cstr));
    cstr = "key.secp256k1.pem";
    String_AddBytes(m, path, bytes(cstr), strlen(cstr));

    String *pubPath = String_Init(m, STRING_EXTEND);
    cstr = "./artifact/fixtures/";
    String_AddBytes(m, pubPath, bytes(cstr), strlen(cstr));
    cstr = "pub.secp256k1.pem";
    String_AddBytes(m, pubPath, bytes(cstr), strlen(cstr));
    
    Secure *pubKey = Secure_PubKey(m, pubPath, NULL);
    r |= Test(pubKey != NULL, "PubKey is non-null");

    String *content = String_Make(m, bytes("and here is the file of wonder\n"));
    String *digest = String_ToHex(m, Digest_Sha256(m, content));

    String *sig = String_FromB64(m, String_Make(m, bytes("MEUCIQDTPncC3wvlQJbnmodkR8VZDBGDCoWqoMkG1akcqV/FjAIgK0UgSXxZHqPaDtNQGeGume8ypLKMW43V6HLbZRarSq0=")));
    status re = Asymetric_Verify(m, content, String_Make(m, bytes("some bozo mismatch not gonna worky signature")), pubKey);
    r |= Test(re & NOOP, "UnSuccessful verification of incorrect ec signature using public key");
    re = Asymetric_Verify(m, content, sig, pubKey);
    r |= Test(re & SUCCESS, "Successful verification of ec signature using public key");


    MemCtx_Free(m);
    DebugStack_Pop();
    return r;
}
