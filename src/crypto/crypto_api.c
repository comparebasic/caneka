#include <external.h>
#include <caneka.h>

String *String_Sha256(MemCtx *m, String *s){
    struct sha256_state _st;
    Sha256_init(&_st);
    byte buff[SHA256_DIGEST_SIZE+1];
    memset(buff, 0, sizeof(buff));

    while(s != NULL){
        Sha256_process(&_st, s->bytes, s->length);
        s = String_Next(s);
    }

    Sha256_done(&_st, buff);
    String *ret = String_Init(m, SHA256_DIGEST_SIZE);
    String_AddBytes(m, ret, buff, SHA256_DIGEST_SIZE);
    return ret;
}
