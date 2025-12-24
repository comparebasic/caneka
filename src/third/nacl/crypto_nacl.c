#include <external.h>
#include <caneka.h>
#include <nacl_module.h>

void randombytes(byte *b, i64 length){
    getrandom(b, length, 0);
}

status Str_ToSha256(MemCh *m, Str *s, digest *hash){
    Sha256Ctx ctx;
    memset(&ctx, 0, sizeof(Sha256Ctx));
    sha256_start(&ctx);
    memset(hash, 0, DIGEST_SIZE);
    sha256_finalize(&ctx, (byte *)hash, s->bytes, s->length);
    return SUCCESS;
}

status StrVec_ToSha256(MemCh *m, StrVec *v, digest *hash){
    Sha256Ctx ctx;
    v = StrVec_ReAlign(m, v);
    memset(&ctx, 0, sizeof(Sha256Ctx));
    sha256_start(&ctx);
    Iter it;
    Iter_Init(&it, v->p);
    while((Iter_Next(&it) & END) == 0){
        Str *s = (Str *)Iter_Get(&it);
        if(it.type.state & LAST){
            sha256_finalize(&ctx, (byte *)hash, s->bytes, s->length);
            return SUCCESS;
        }else{
            sha256_update(&ctx, s->bytes, s->length);
        }
    }
    return ERROR;
}

status StrVec_SaltedDigest(MemCh *m,
        StrVec *_v, Str *salt, digest *hash, util nonce){
    /* This places the nonce and value inside the salt
     * positioned by the highest power of two from the
     * nonce
     *
     * For example: 
     * nonce = 95438134
     * salt-length = 512
     * 
     * The first 134 bytes of the salt, will be followed by
     * the nonce, the value of v, and then the remaining
     * 378 bytes of the salt. 
     *
     * To find a position within the salt length it tries
     * powers of two by shifting a bit. The subtracts 1 from
     * that to make all 1s below the maximum position.
     *
     * For example:
     *    0 = 0
     *    100 = 11 = 3
     *    1000 = 111 = 7
     *    1000000000 = 111111111 = 511
     *
     * For the above nonce, 95438134 & 511 = 134, which is then
     * used as the position.
     *
     * That is then sent through Sha256 to create the digest.
     */
    
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
        &nonce, sizeof(util), sizeof(util), STRING_BINARY|STRING_COPY);
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

status SignPair_PublicFromPem(Buff *bf, Str *public){
    Error(bf->m, FUNCNAME, FILENAME, LINENUMBER, 
        "Not Implemented", NULL);
    return ERROR;
}

status SignPair_PrivateFromPem(Buff *bf, Str *secret){
    Error(bf->m, FUNCNAME, FILENAME, LINENUMBER, 
        "Not Implemented", NULL);
    return ERROR;
}

status SignPair_Make(MemCh *m, Str *public, Str *secret, StrVec *phrase){
    if(public->alloc != secret->alloc && public->alloc != DIGEST_SIZE ||
        phrase == NULL || phrase->total == 0 ){
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Error public and secret Str objects must be exactly DIGEST_SIZE allocations, and the phrase must have content",
            NULL);
        return ERROR;
    }
    Str *hash = Str_Make(m, DIGEST_SIZE);
    StrVec_ToSha256(m, phrase, (digest *)hash->bytes);
    sign_keypair_sha256(public->bytes, secret->bytes, hash->bytes);
    public->length = DIGEST_SIZE;
    secret->length = DIGEST_SIZE;
    return SUCCESS;
}

Str *SignPair_Sign(MemCh *m, Str *content, Str *secret){
    Str *sig = Str_Make(m, content->length+SIG_FOOTER_SIZE); 
    crypto_uint64 length = 0;
    crypto_sign(sig->bytes, &length, content->bytes, content->length, secret->bytes);
    sig->length = length;
    return sig;
}

status SignPair_Verify(MemCh *m, Str *content, Str *sig, Str *public){
    crypto_uint64 length = 0;
    i32 r = crypto_sign_open(
        content->bytes, &length, sig->bytes, sig->length, public->bytes);
    return r == 0 ?
        SUCCESS:
        NOOP;
}

status BoxPair_Make(MemCh *m, Str *public, Str *secret, StrVec *phrase){
    Error(m, FUNCNAME, FILENAME, LINENUMBER, 
        "Not Implemented", NULL);
    return ERROR;
}

status BoxPair_Enc(MemCh *m, Str *secret, StrVec *content){
    Error(m, FUNCNAME, FILENAME, LINENUMBER, 
        "Not Implemented", NULL);
    return ERROR;
}

status BoxPair_Dec(MemCh *m, Str *public, StrVec *content){
    Error(m, FUNCNAME, FILENAME, LINENUMBER, 
        "Not Implemented", NULL);
    return ERROR;
}

status Str_HmacEnc(MemCh *m, Str *s, Str *hmac){
    Error(m, FUNCNAME, FILENAME, LINENUMBER, 
        "Not Implemented", NULL);
    return ERROR;
}

status Str_HmacDec(MemCh *m, Str *s, Str *hmac){
    Error(m, FUNCNAME, FILENAME, LINENUMBER, 
        "Not Implemented", NULL);
    return ERROR;
}

status Crypto_Init(MemCh *m){
    return SUCCESS;
}
