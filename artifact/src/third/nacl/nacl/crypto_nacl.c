#include <external.h>
#include <caneka.h>
#include <cnk_nacl_crypto.h>

void randombytes(byte *b, i64 length){
    getrandom(b, length, 0);
}

Str *Str_ToSha256(MemCh *m, Str *s){
    Sha256Ctx ctx;
    memset(&ctx, 0, sizeof(Sha256Ctx));
    sha256_start(&ctx);
    Str *d = Str_Make(m, DIGEST_SIZE);
    sha256_finalize(&ctx, d->bytes, s->bytes, s->length);
    d->length = DIGEST_SIZE;
    return d;
}

Str *StrVec_ToSha256(MemCh *m, StrVec *v){
    Sha256Ctx ctx;
    Abstract *args[3];
    v = StrVec_ReAlign(m, v);
    memset(&ctx, 0, sizeof(Sha256Ctx));
    sha256_start(&ctx);
    Str *d = Str_Make(m, DIGEST_SIZE);
    Iter it;
    Iter_Init(&it, v->p);
    while((Iter_Next(&it) & END) == 0){
        Str *s = (Str *)Iter_Get(&it);
        if(it.type.state & LAST){
            sha256_finalize(&ctx, d->bytes, s->bytes, s->length);
            d->length = DIGEST_SIZE;
            return d;
        }else{
            sha256_update(&ctx, s->bytes, s->length);
        }
    }
    v->type.state |= ERROR;
    return NULL;
}

Str *StrVec_SaltedDigest(MemCh *m, StrVec *orig, Str *salt){
    StrVec *v = (StrVec *)StrVec_Clone(m, (Abstract *)orig);
    StrVec_AddBytes(m, v, salt->bytes, salt->length);
    return StrVec_ToSha256(m, v);
}

Str *Str_DigestAlloc(MemCh *m){
    return Str_Make(m, DIGEST_SIZE);
}

status SignPair_Make(MemCh *m, Str *public, Str *secret, StrVec *phrase){
    if(public->alloc != secret->alloc && public->alloc != DIGEST_SIZE ||
        phrase == NULL || phrase->total == 0 ){
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Error public and secret Str objects must be exactly DIGEST_SIZE allocations, and the phrase must have content",
            NULL);
        return ERROR;
    }
    Str *digest = StrVec_ToSha256(m, phrase);
    sign_keypair_sha256(public->bytes, secret->bytes, digest->bytes);
    public->length = DIGEST_SIZE;
    secret->length = DIGEST_SIZE;
    return SUCCESS;
}

Str *SignPair_Sign(MemCh *m, Str *content, Str *secret){
    Str *sig = Str_Make(m, content->length+SIGNATURE_SIZE); 
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
