#include <external.h>
#include <caneka.h>

status Str_ToSha256(MemCh *m, Str *s, digest *hash){
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    if(mdctx == NULL){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "Error allocating evp_md_ctx", NULL);
        return ERROR;
    }

    if(1 != EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL)){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "Error initializing sha256", NULL);
        return ERROR;
    }

    if(1 != EVP_DigestUpdate(mdctx, s->bytes, s->length)){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "Error updating sha256", NULL);
        return ERROR;
    }

    unsigned int len = DIGEST_SIZE;
	if(1 != EVP_DigestFinal(mdctx, (byte *)hash, &len)){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "Error finalizing sha256 digest", NULL);
        return ERROR;
    }

    EVP_MD_CTX_free(mdctx);
    if(len == DIGEST_SIZE){
        return SUCCESS;
    }else{
        return NOOP;
    }
}

status StrVec_ToSha256(MemCh *m, StrVec *v, digest *hash){
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    if(mdctx == NULL){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "Error allocating evp_md_ctx", NULL);
        return ERROR;
    }

    if(1 != EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL)){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "Error initializing sha256", NULL);
        return ERROR;
    }

    Iter it;
    Iter_Init(&it, v->p);
    while((Iter_Next(&it) & END) == 0){
        Str *s = Iter_Get(&it);
        if(1 != EVP_DigestUpdate(mdctx, s->bytes, s->length)){
            Fatal(FUNCNAME, FILENAME, LINENUMBER, "Error updating sha256", NULL);
            return ERROR;
        }
    }

    unsigned int len = DIGEST_SIZE;
	if(1 != EVP_DigestFinal(mdctx, (byte *)hash, &len)){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "Error finalizing sha256 digest", NULL);
        return ERROR;
    }

    EVP_MD_CTX_free(mdctx);
    if(len == DIGEST_SIZE){
        return SUCCESS;
    }else{
        return NOOP;
    }
}

status StrVec_SaltedDigest(MemCh *m,
        StrVec *_v, Str *salt, digest *hash, util nonce){
    
    i32 sep = 1;
    i32 next = sep;
    while((next = (next << 1)) <= salt->length){
        sep = next;
    }
    sep--;

    word div = nonce & sep;

    Str *salt1 = Str_Copy(m, salt);
    salt1->length = div;
    StrVec *v = StrVec_From(m, salt1);

    Str *ns = Str_Ref(m,
        (byte *)&nonce, sizeof(util), sizeof(util), STRING_BINARY|STRING_COPY);
    StrVec_Add(v, ns);

    StrVec_AddVec(v, _v);

    Str *salt2 = Str_Copy(m, salt);
    salt->length = salt->length - div;
    StrVec_Add(v, salt2);

    return StrVec_ToSha256(m, v, hash);
}

Str *Str_DigestAlloc(MemCh *m){
    return Str_Make(m, DIGEST_SIZE);
}

status SignPair_Make(MemCh *m, Single *public, Single *secret){
    EVP_PKEY *key = EVP_EC_gen("P-256");

    if(key == NULL){
        OpenSsl_Error(ErrStream);
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Error generating secret key", NULL);
        return ERROR;
    }
    secret->val.ptr = key;
    secret->objType.of = TYPE_ECKEY;

    util sz = 0;
    Str *s = Str_MakeBlank(m);
    EVP_PKEY_get_octet_string_param(key, "pub", NULL, 0, &sz);
    s->bytes = OPENSSL_malloc(sz);

    if(1 != EVP_PKEY_get_octet_string_param(key, "pub", s->bytes, sz, &sz)){
        OpenSsl_Error(ErrStream);
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Error generating public key", NULL);
        return ERROR;
    }

    s->length = sz;
    public->val.ptr = s;
    public->objType.of = TYPE_STR;
    MemCh_AddExtFree(m, Ptr_Wrapped(m, s->bytes, TYPE_BYTES_POINTER));

    return SUCCESS;
}

status SignPair_PublicFromPem(Buff *bf, Single *public){
    public->val.ptr = EVP_PKEY_new();
    public->objType.of = TYPE_ECKEY_PUB;
    MemCh_AddExtFree(bf->m, public);

    Buff_Read(bf);
    Str *s = StrVec_Str(bf->m, bf->v);

    if(d2i_PublicKey(EVP_PKEY_ED25519,
                (EVP_PKEY **)public->val.ptr,
                (const unsigned char **)&s->bytes,
                s->length)
            == NULL){
        Error(bf->m, FUNCNAME, FILENAME, LINENUMBER,
            "Unable to read public key", NULL);
        return ERROR;
    }
    return SUCCESS;
}

Str *SignPair_Sign(MemCh *m, StrVec *content, Single *secret){
    status r = READY;
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    if(mdctx == NULL){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "Error allocating evp_md_ctx", NULL);
        return NULL;
    }
    EVP_PKEY *key = secret->val.ptr;
    if(key == NULL){
        Fatal(FUNCNAME, FILENAME, LINENUMBER,
            "Error allocating private key for use", NULL);
        return NULL;
    }

    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_EC,NULL);
    if(1 != EVP_DigestSignInit(mdctx, &ctx, EVP_sha256(), NULL, key)){
        OpenSsl_Error(ErrStream);
        Fatal(FUNCNAME, FILENAME, LINENUMBER,
            "Error initializing digest verify", NULL);
        return NULL;
    }

    Iter it;
    Iter_Init(&it, content->p);
    while((Iter_Next(&it) & END) == 0){
        Str *s = (Str *)Iter_Get(&it);
        if(1 != EVP_DigestSignUpdate(mdctx, s->bytes, s->length)){
            Fatal(FUNCNAME, FILENAME, LINENUMBER, "Error updating digest verify", NULL);
            return NULL;
        }
    }

    size_t len;
	if(1 != EVP_DigestSignFinal(mdctx, NULL, &len)){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "Error finalizing digest verify", NULL);
        return NULL;
    }
    if(len > STR_DEFAULT){
        Fatal(FUNCNAME, FILENAME, LINENUMBER,
            "Error allocating digest verify, longer than string", NULL);
        return NULL;
    }

    Str *ret = Str_Make(m, len);
	if(1 != EVP_DigestSignFinal(mdctx, ret->bytes, &len)){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "Error finalizing copying digest verify", NULL);
        return NULL;
    }
    ret->length = len;
    ret->type.state |= STRING_BINARY;

    EVP_PKEY_free(key);
    EVP_MD_CTX_destroy(mdctx);
    return ret;
}

status SignPair_Verify(MemCh *m, StrVec *content, Str *sig, Single *public){
    status r = READY;
    Str *s = public->val.ptr;

    EVP_PKEY *key = EVP_PKEY_new();
    OSSL_PARAM *params;
    OSSL_PARAM_BLD *param_bld = OSSL_PARAM_BLD_new();
    OSSL_PARAM_BLD_push_utf8_string(param_bld, "group", "P-256", 0);
    OSSL_PARAM_BLD_push_octet_string(param_bld, "pub", s->bytes, s->length);
    params = OSSL_PARAM_BLD_to_param(param_bld);
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new_from_name(NULL, "EC", NULL);
    if(ctx == NULL || 
            !EVP_PKEY_fromdata_init(ctx) || 
            !EVP_PKEY_fromdata(ctx, &key, EVP_PKEY_PUBLIC_KEY, params)
        ){
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Error creating public key from octets", NULL);
        return ERROR;
    }

    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    if(mdctx == NULL){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "Error allocating evp_md_ctx", NULL);
        return ERROR;
    }

    if(1 != EVP_DigestVerifyInit(mdctx, NULL, EVP_sha256(), NULL, key)){
        Fatal(FUNCNAME, FILENAME, LINENUMBER,
            "Error initializing digest verify", NULL);
        return NOOP;
    }

    Iter it;
    Iter_Init(&it, content->p);
    while((Iter_Next(&it) & END) == 0){
        Str *s = Iter_Get(&it);
        if(1 != EVP_DigestVerifyUpdate(mdctx, s->bytes, s->length)){
            Fatal(FUNCNAME, FILENAME, LINENUMBER,
                "Error updating digest verify", NULL);
            return NOOP;
        }
    }

	if(1 != EVP_DigestVerifyFinal(mdctx, sig->bytes, sig->length)){
        printf("Invalid!\n");
        return NOOP;
    }

    return SUCCESS;
}

status BoxPair_Make(MemCh *m, Str *public, Str *secret, StrVec *phrase){
    return NOOP;
}

status BoxPair_Enc(MemCh *m, Str *secret, StrVec *content){
    return NOOP;
}

status BoxPair_Dec(MemCh *m, Str *public, StrVec *content){
    return NOOP;
}

status Str_HmacEnc(MemCh *m, Str *s, Str *hmac){
    return NOOP;
}

status Str_HmacDec(MemCh *m, Str *s, Str *hmac){
    return NOOP;
}
