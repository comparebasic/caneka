#include <basictypes_external.h> 
#include <caneka/caneka.h> 

String *Crypto_ToSha256(MemCtx *m, String *s){
    return s;
}

lifecycle_t Crypto_GenerateKeyPair(MemCtx *m, String *pk, String *sk, unsigned int nbits){
    return SUCCESS;
}

lifecycle_t *Crypto_LoadPulicKey(MemCtx *m, String *path){
    return NULL;
}

lifecycle_t *Crypto_LoadPrivateKey(MemCtx *m, String *pk_path, String *sk_path){
    return NULL;
}

String *Crypto_SkEncrypt(MemCtx *m, String *pk, String *content){
    return  NULL;
}

String *Crypto_SkPkDecrypt(MemCtx *m, String *pk, String *sk, String *enc_content){
    return NULL;
}

String *Crypto_SignText(MemCtx *m, String *sk, String *content){
    return NULL;
}

lifecycle_t Crypto_VerifySignature(MemCtx *m, String *pk, String *content, String *signature){
    return ERROR;
}
