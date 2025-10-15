#include <external.h>
#include <caneka.h>
#include <cnk_nacl_crypto.h>

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

Str *Str_SaltedDigest(MemCh *m, Str *s, Str *salt){
    return NULL;;
}

status KeyPair_Make(MemCh *m, Str *public, Str *secret, StrVec *phrase){
    return NOOP;
}

StrVec *KeyPair_Sign(MemCh *m, Str *secret, StrVec *content){
    return NULL;
}

status KeyPair_Verify(MemCh *m, Str *public, StrVec *content){
    return NOOP;
}

status Str_HmacEnc(MemCh *m, Str *s, Str *hmac){
    return NOOP;
}

status Str_HmacDec(MemCh *m, Str *s, Str *hmac){
    return NOOP;
}
