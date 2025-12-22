#include <external.h> 
#include <caneka.h> 

StrVec *Salty_Enc(MemCh *m, Str *key, StrVec *v){
    if((key->type.state & STRING_BINARY) == 0){
        key = Digest_Sha256(m, StrVec_From(m, key));
    }

    return Symetric_Enc(m, key, v);
}

StrVec *Salty_Dec(MemCh *m, Str *key, StrVec *v){
    if((key->type.state & STRING_BINARY) == 0){
        key = Digest_Sha256(m, StrVec_From(m, key));
    }

    return Symetric_Dec(m, key, v);
}
