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
        Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "Error updating sha256");
        return ERROR;
    }

    unsigned int len;
	if(1 != EVP_DigestFinal(mdctx, hash, &len)){
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

    unsigned int len;
	if(1 != EVP_DigestFinal(mdctx, hash, &len)){
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
        StrVec *v, Str *salt, digest *hash, util nonce){

    StrVec *copy = StrVec_Copy(m, v);
    
    i32 sep = 1;
    i32 next = sep;
    while((next = (next << 1)) <= salt->length){
        sep = next;
    }
    sep--;

    word div = util & sep;

    Str *salt1 = Str_Copy(m, salt);
    salt1->length = div;
    StrVec *v = StrVec_From(m, salt1);
    StrVec_AddVec(v, _v);
    Str *salt2 = Str_Copy(m, salt);
    salt->length = salt->length - div;
    StrVec_Add(v, salt2);

    return StrVec_ToSha256(m, v, hash);
}

Str *Str_DigestAlloc(MemCh *m){
    return Str_Make(m, DIGEST_SIZE);
}

status SignPair_Make(MemCh *m, Str *public, Str *secret, StrVec *phrase){
    if(phrase != NULL){
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Not implemented, OpenSSL Caneka bindings presently only generate"
            " random keys.", NULL);
        return ERROR;
    }

    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_ED25519, NULL);
    if(ctx == NULL){
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Error secret ctx not allocated", NULL);
        return ERROR;
    }

    EVP_KEY *key = EVP_PKEY_new();
    
    if(1 != EVP_PKEY_keygen_init(ctx) || 1 != EVP_PKEY_keygen(ctx, key)){
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Error generating key", NULL);
        return ERROR;
    }

    size_t len = 0;
    EVP_PKEY_get_raw_private_key(key, NULL, &len);
    if(secret->alloc < len){
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Error secret key length too short to fill", NULL);
        return ERROR;
    }

    len = secret->alloc;
    if(1 != EVP_PKEY_get_raw_private_key(key, secret->bytes, &len)){
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Error creating new private key", NULL);
        return ERROR;
    }
    secret->length = len;

    len = 0;
    EVP_PKEY_get_raw_public_key(key, NULL, &len);
    if(public->alloc < len){
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Error public key length too short to fill", NULL);
        return ERROR;
    }

    len = public->alloc;
    if(1 != EVP_PKEY_get_raw_public_key(key, public->bytes, &len)){
        return ERROR;
    }
    public->length = len;
    return SUCCESS;
}

status SignPair_PublicFromPem(Buff *bf, Str *public){
    Error(bf->m, FUNCNAME, FILENAME, LINENUMBER, 
        "Not Implemented", NULL);
    return ERROR;
}

status SignPair_PrivateFromPem(Buff *bf, Str *secret){
    Str *s = StrVec_Str(bf->m, bf->v);
    BIO *bio = BIO_new_mem_buff(s->bytes, s->length);
    if(bio == NULL){
        Error(bf->m, FUNCNAME, FILENAME, LINENUMBER,
            "Unable to allocate bio to read private key pem", NULL);
        return ERROR;
    }
    char *name = NULL;
    char *header = NULL;
    util datalen = 0;
    if(PEM_read_bio(bio, &name, &header, &secret->bytes, &datalen) == 1){
        secret->length = secret->alloc = datalen;
        MemCh_AddExtFree(m, Ptr_Wrapped(m, secret->bytes, TYPE_ECKEY));
        if(name != NULL) OPENSSL_free(name);
        if(header != NULL) OPENSSL_free(header);
        return SUCCESS;
    }
    return ERROR;
}

status SignPair_PublicFromPem(Buff *bf, Str *public){
    Str *s = StrVec_Str(bf->m, bf->v);
    BIO *bio = BIO_new_mem_buff(s->bytes, s->length);
    if(bio == NULL){
        Error(bf->m, FUNCNAME, FILENAME, LINENUMBER,
            "Unable to allocate bio to read private key pem", NULL);
        return ERROR;
    }
    char *name = NULL;
    char *header = NULL;
    util datalen = 0;
    if(PEM_read_bio(bio, &name, &header, &secret->bytes, &datalen) == 1){
        secret->length = secret->alloc = datalen;
        MemCh_AddExtFree(m, Ptr_Wrapped(m, secret->bytes, TYPE_ECKEY));
        if(name != NULL) OPENSSL_free(name);
        if(header != NULL) OPENSSL_free(header);
        return SUCCESS;
    }
    return ERROR;
}

Str *SignPair_Sign(MemCh *m, Str *content, Single *secret){
    status r = READY;
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    if(mdctx == NULL){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "Error allocating evp_md_ctx", NULL);
        return NULL;
    }
    EVP_KEY *key = d2i_AutoPrivateKey(NULL, secret->bytes, secret->length);
    if(key == NULL){
        Fatal(FUNCNAME, FILENAME, LINENUMBER,
            "Error allocating private key for use", NULL);
        return NULL;
    }

    if(1 != EVP_DigestSignInit(mdctx, NULL, EVP_sha256(), NULL, key)){
        Fatal(FUNCNAME, FILENAME, LINENUMBER,
            "Error initializing digest verify", NULL);
        return NULL;
    }

    Iter it;
    Iter_Init(&it, v->p);
    while((Iter_Next(&it) & END) == 0){
        Str *s = (Str *)it.value;
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

status SignPair_Verify(MemCh *m, Str *content, Str *sig, Str *public){
    status r = READY;
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    if(mdctx == NULL){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "Error allocating evp_md_ctx", NULL);
        return ERROR;
    }
    EVP_KEY *key = i2d_PublicKey(NULL, public->bytes, public->length);
    if(key == NULL){
        Fatal(FUNCNAME, FILENAME, LINENUMBER,
            "Error allocating private key for use", NULL);
        return ERROR;
    }

    if(1 != EVP_DigestVerifyInit(mdctx, NULL, EVP_sha256(), NULL, key)){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "Error initializing digest verify", NULL);
        return NOOP;
    }

    Iter it;
    Iter_Init(&it, v->p);
    while((Iter_Next(&it) & END) == 0){
        Str *s = it.value;
        if(1 != EVP_DigestVerifyUpdate(mdctx, s->bytes, s->length)){
            Fatal(FUNCNAME, FILENAME, LINENUMBER, "Error updating digest verify", NULL);
            return NOOP;
        }
    }

	if(1 != EVP_DigestVerifyFinal(mdctx, sig->bytes, sig->alloc)){
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
