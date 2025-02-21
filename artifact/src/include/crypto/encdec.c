#include <external.h>
#include <caneka.h>

status EncStr(MemCtx *m, String *key, String *content){
    if(key->length != 64){
        Fatal("Must have 64 byte/256 bit string key, did you forget to sha256 the string key?", 0);
    }
    /* encrypt with aes */
    return NOOP;
}

status EncStr(MemCtx *m, String *key, String *content){
    if(key->length != 64){
        Fatal("Must have 64 byte/256 bit string key, did you forget to sha256 the string key?", 0);
    }
    /* decrypt with aes */
    return NOOP;
    return NOOP;
}
