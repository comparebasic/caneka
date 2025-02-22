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

    unsigned char *digest;
    size_t sz = EVP_MD_size(EVP_sha256());
    if((digest = (unsigned char *)OPENSSL_malloc(sz)) == NULL){
        Fatal("Error allocating sha256 digest", TYPE_CRYPTO_DIGEST);
        DebugStack_Pop();
        return NULL;
    }

    unsigned int len;
	if(1 != EVP_DigestFinal_ex(mdctx, digest, &len)){
        Fatal("Error finalizing sha256 digest", TYPE_CRYPTO_DIGEST);
        DebugStack_Pop();
        return NULL;
    }

    String *ret = String_Init(m, STRING_EXTEND);
    String_AddBytes(m, ret, bytes(digest), len);
    ret->type.state |= FLAG_STRING_BINARY;

    EVP_MD_CTX_free(mdctx);
    DebugStack_Pop();
    return ret;
}
