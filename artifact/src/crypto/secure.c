#include <external.h>
#include <caneka.h>

status Secure_Free(MemCtx *m, Secure *sc){
    if(sc->type.of == TYPE_SECURE_PUBKEY){
        EVP_PKEY_free(sc->ptr);
        return SUCCESS;
    }else{
        return NOOP;
    }
}

Secure *Secure_PubKey(MemCtx *m, String *fname, Access *access){
    Secure *sc = (Secure *)MemCtx_Alloc(m, sizeof(Secure));
    sc->type.of = TYPE_SECURE_PUBKEY;
    File file;
    File_Init(&file, fname, access, NULL);
    file.abs = File_GetAbsPath(m, file.path);
    FILE *fp = File_GetFILE(m, &file, access);
    if(fp == NULL){
        Fatal("unable to open file for pubkey", TYPE_SECURE_PUBKEY);
        return NULL;
    }
    sc->ptr = (void *)PEM_read_PUBKEY(fp, (EVP_PKEY **)&sc->ptr, NULL, NULL);
    if(sc->ptr == NULL){
        Fatal("unable to interpret pubkey", TYPE_SECURE_PUBKEY);
        return NULL;
    }
    return sc;
}

Secure *Secure_Key(MemCtx *m, String *fname, Access *access){
    Secure *sc = (Secure *)MemCtx_Alloc(m, sizeof(Secure));
    sc->type.of = TYPE_SECURE_PRIVKEY;
    File file;
    File_Init(&file, fname, access, NULL);
    file.abs = File_GetAbsPath(m, file.path);
    FILE *fp = File_GetFILE(m, &file, access);
    if(fp == NULL){
        Fatal("unable to open file for private key", TYPE_SECURE_PRIVKEY);
        return NULL;
    }
    sc->ptr = (void *)PEM_read_PrivateKey(fp, (EVP_PKEY **)&sc->ptr, NULL, NULL);
    if(sc->ptr == NULL){
        Fatal("unable to interpret private key", TYPE_SECURE_PRIVKEY);
        return NULL;
    }
    return sc;
}
