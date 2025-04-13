#include <external.h>
#include <caneka.h>

Secure *Digest_Sha256Init(MemCh *m){
    return NULL;
}

status Digest_Sha256Update(Secure *sha, Str *s){
    return NOOP;
}

Str *Digest_Sha256Final(MemCh *m){
    return NULL;
}

Str *Digest_Sha256(MemCh *m, StrVec *v){
    DebugStack_Push(NULL, 0);
    EVP_MD_CTX *mdctx;
    mdctx = EVP_MD_CTX_new();
    if(mdctx == NULL){
        Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "Error allocating evp_md_ctx");
        DebugStack_Pop();
        return NULL;
    }

    if(1 != EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL)){
        Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "Error initializing sha256");
        DebugStack_Pop();
        return NULL;
    }

    Iter it;
    Iter_Init(&it, v->p);
    while((Iter_Next(&it) & END) == 0){
        Str *s = (Str *)it.value;
        if(1 != EVP_DigestUpdate(mdctx, s->bytes, s->length)){
            Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "Error updating sha256");
            DebugStack_Pop();
            return NULL;
        }
    }

    size_t sz = EVP_MD_size(EVP_sha256());
    Str *ret = Str_Make(m, STR_DEFAULT);
    if(sz > STR_DEFAULT){
        Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "Error allocating sha256 digest, longer than string");
        DebugStack_Pop();
        return NULL;
    }

    unsigned int len;
	if(1 != EVP_DigestFinal(mdctx, ret->bytes, &len)){
        Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "Error finalizing sha256 digest");
        DebugStack_Pop();
        return NULL;
    }

    ret->length = len;
    ret->type.state |= STRING_BINARY;

    EVP_MD_CTX_free(mdctx);
    DebugStack_Pop();
    return ret;
}
