#include <external.h> 
#include <caneka.h> 

String *Salty_MakeKey(MemCtx *m, String *s){
    return Digest_Sha256(m, s);
}

status Salty_Enc(MemCtx *m, String *key, String *s){
    return SUCCESS;
}

status Salty_Dec(MemCtx *m, String *key, String *s){
    return SUCCESS;
}
