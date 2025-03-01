#include <external.h>
#include <caneka.h>

Secure *Asymetric_VerifyInit(MemCtx *m, String *s, String *sig, Secure *pubKey){
    return NULL;
}

status Asymetric_VerifyUpdate(Secure *asym, String *s){
    return NOOP;
}

Secure *Asymetric_VerifyFinal(Secure *asym, String *s){
    return NULL;
}

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

String *Asymetric_Sign(MemCtx *m, String *s, Secure *Key){
    DebugStack_Push(NULL, 0);
    status r = READY;
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    Key = as(Key, TYPE_SECURE_PRIVKEY);
    EVP_PKEY *key = (EVP_PKEY *)Key->ptr;
    if(mdctx == NULL){
        Fatal("Error allocating evp_md_ctx", TYPE_CRYPTO_DIGEST);
        DebugStack_Pop();
        return NULL;
    }

    if(1 != EVP_DigestSignInit(mdctx, NULL, EVP_sha256(), NULL, key)){
        Fatal("Error initializing digest verify", TYPE_CRYPTO_DIGEST);
        DebugStack_Pop();
        return NULL;
    }

    while(s != NULL){
        if(1 != EVP_DigestSignUpdate(mdctx, s->bytes, s->length)){
            Fatal("Error updating digest verify", TYPE_CRYPTO_DIGEST);
            DebugStack_Pop();
            return NULL;
        }
        s = String_Next(s);
    }

    size_t len;
	if(1 != EVP_DigestSignFinal(mdctx, NULL, &len)){
        DebugStack_Pop();
        Fatal("Error finalizing digest verify", TYPE_CRYPTO_DIGEST);
        return NULL;
    }
    String *ret = String_Init(m, STRING_EXTEND);
    if(len > String_GetSegSize(ret)){
        Fatal("Error allocating digest verify, longer than string", TYPE_CRYPTO_DIGEST);
        DebugStack_Pop();
        return NULL;
    }
	if(1 != EVP_DigestSignFinal(mdctx, ret->bytes, &len)){
        DebugStack_Pop();
        Fatal("Error finalizing copying digest verify", TYPE_CRYPTO_DIGEST);
        return NULL;
    }
    ret->length = len;
    ret->type.state |= FLAG_STRING_BINARY;

    EVP_MD_CTX_destroy(mdctx);
    DebugStack_Pop();
    return ret;
}
