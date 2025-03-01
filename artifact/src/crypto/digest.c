#include <external.h>
#include <caneka.h>

String *Digest_Sha256(MemCtx *m, String *s){
    DebugStack_Push(NULL, 0);
    EVP_MD_CTX *mdctx;
    mdctx = EVP_MD_CTX_new();
    if(mdctx == NULL){
        Fatal("Error allocating evp_md_ctx", TYPE_CRYPTO_DIGEST);
        DebugStack_Pop();
        return NULL;
    }

    if(1 != EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL)){
        Fatal("Error initializing sha256", TYPE_CRYPTO_DIGEST);
        DebugStack_Pop();
        return NULL;
    }

    while(s != NULL){
        if(1 != EVP_DigestUpdate(mdctx, s->bytes, s->length)){
            Fatal("Error updating sha256", TYPE_CRYPTO_DIGEST);
            DebugStack_Pop();
            return NULL;
        }
        s = String_Next(s);
    }

    size_t sz = EVP_MD_size(EVP_sha256());
    String *ret = String_Init(m, STRING_EXTEND);
    if(sz > String_GetSegSize(ret)){
        Fatal("Error allocating sha256 digest, longer than string", TYPE_CRYPTO_DIGEST);
        DebugStack_Pop();
        return NULL;
    }

    unsigned int len;
	if(1 != EVP_DigestFinal(mdctx, ret->bytes, &len)){
        Fatal("Error finalizing sha256 digest", TYPE_CRYPTO_DIGEST);
        DebugStack_Pop();
        return NULL;
    }

    ret->length = len;
    ret->type.state |= FLAG_STRING_BINARY;

    EVP_MD_CTX_free(mdctx);
    DebugStack_Pop();
    return ret;
}
