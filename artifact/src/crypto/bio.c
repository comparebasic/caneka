#include <external.h>
#include <caneka.h>

Secure *Crypto_Bio(MemCtx *m, String *s){
    Secure *sc = (Secure *)MemCtx_Alloc(m, sizeof(Secure));
    sc->type.of = TYPE_SECURE_BIO;
}
