#include <external.h> 
#include <caneka.h> 

String *Salty_Enc(MemCtx *m, String *key, String *s){
    if((key->type.state & FLAG_STRING_BINARY) == 0){
        key = Digest_Sha256(m, key);
    }

    return Symetric_Enc(m, key, s);
}

String *Salty_Dec(MemCtx *m, String *key, String *s){
    if((key->type.state & FLAG_STRING_BINARY) == 0){
        key = Digest_Sha256(m, key);
    }

    return Symetric_Dec(m, key, s);
}
