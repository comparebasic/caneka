#include <external.h>
#include <caneka.h>

status Asymetric_Verify(MemCtx *m, String *s, String *sig, Secure *pubKey){
    DebugStack_Push(NULL, 0);
    status r = READY;
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    pubKey = as(pubKey, TYPE_SECURE_PUBKEY);
    EVP_PKEY *key = (EVP_PKEY *)pubKey->ptr;
    if(mdctx == NULL){
        Fatal("Error allocating evp_md_ctx", TYPE_CRYPTO_DIGEST);
        DebugStack_Pop();
        return ERROR;
    }

    if(1 != EVP_DigestVerifyInit(mdctx, NULL, EVP_sha256(), NULL, key)){
        Fatal("Error initializing digest verify", TYPE_CRYPTO_DIGEST);
        DebugStack_Pop();
        return NOOP;
    }

    while(s != NULL){
        if(1 != EVP_DigestVerifyUpdate(mdctx, s->bytes, s->length)){
            Fatal("Error updating digest verify", TYPE_CRYPTO_DIGEST);
            DebugStack_Pop();
            return NOOP;
        }
        s = String_Next(s);
    }

	if(1 != EVP_DigestVerifyFinal(mdctx, sig->bytes, sig->length)){
        DebugStack_Pop();
        return NOOP;
    }

    DebugStack_Pop();
    return SUCCESS;
}
