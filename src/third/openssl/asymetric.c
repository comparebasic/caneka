#include <external.h>
#include <caneka.h>

Secure *Asymetric_VerifyInit(MemCh *m, StrVec *v, Str *sig, Secure *pubKey){
    return NULL;
}

status Asymetric_VerifyUpdate(Secure *asym, Str *s){
    return NOOP;
}

Secure *Asymetric_VerifyFinal(Secure *asym, Str *s){
    return NULL;
}

status Asymetric_Verify(MemCh *m, StrVec *v, Str *sig, Secure *pubKey){
    DebugStack_Push(NULL, 0);
    status r = READY;
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    pubKey = as(pubKey, TYPE_SECURE_PUBKEY);
    EVP_PKEY *key = (EVP_PKEY *)pubKey->ptr;
    if(mdctx == NULL){
        Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "Error allocating evp_md_ctx");
        DebugStack_Pop();
        return ERROR;
    }

    if(1 != EVP_DigestVerifyInit(mdctx, NULL, EVP_sha256(), NULL, key)){
        Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "Error initializing digest verify");
        DebugStack_Pop();
        return NOOP;
    }

    Iter it;
    Iter_Init(&it, v->p);
    while((Iter_Next(&it) & END) == 0){
        Str *s = it.value;
        if(1 != EVP_DigestVerifyUpdate(mdctx, s->bytes, s->length)){
            Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "Error updating digest verify");
            DebugStack_Pop();
            return NOOP;
        }
    }

	if(1 != EVP_DigestVerifyFinal(mdctx, sig->bytes, sig->length)){
        DebugStack_Pop();
        return NOOP;
    }

    DebugStack_Pop();
    return SUCCESS;
}

Str *Asymetric_Sign(MemCh *m, StrVec *v, Secure *Key){
    DebugStack_Push(NULL, 0);
    status r = READY;
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    Key = as(Key, TYPE_SECURE_PRIVKEY);
    EVP_PKEY *key = (EVP_PKEY *)Key->ptr;
    if(mdctx == NULL){
        Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "Error allocating evp_md_ctx");
        DebugStack_Pop();
        return NULL;
    }

    if(1 != EVP_DigestSignInit(mdctx, NULL, EVP_sha256(), NULL, key)){
        Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "Error initializing digest verify");
        DebugStack_Pop();
        return NULL;
    }

    Iter it;
    Iter_Init(&it, v->p);
    while((Iter_Next(&it) & END) == 0){
        Str *s = (Str *)it.value;
        if(1 != EVP_DigestSignUpdate(mdctx, s->bytes, s->length)){
            Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "Error updating digest verify");
            DebugStack_Pop();
            return NULL;
        }
    }

    size_t len;
	if(1 != EVP_DigestSignFinal(mdctx, NULL, &len)){
        DebugStack_Pop();
        Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "Error finalizing digest verify");
        return NULL;
    }
    Str *ret = Str_Make(m, STR_DEFAULT);
    if(len > STR_DEFAULT){
        Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "Error allocating digest verify, longer than string");
        DebugStack_Pop();
        return NULL;
    }
	if(1 != EVP_DigestSignFinal(mdctx, ret->bytes, &len)){
        DebugStack_Pop();
        Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "Error finalizing copying digest verify");
        return NULL;
    }
    ret->length = len;
    ret->type.state |= STRING_BINARY;

    EVP_MD_CTX_destroy(mdctx);
    DebugStack_Pop();
    return ret;
}
