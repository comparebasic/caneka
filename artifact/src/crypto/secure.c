#include <external.h>
#include <caneka.h>

status Secure_Free(MemCh *m, Secure *sc){
    if(sc->type.of == TYPE_SECURE_PUBKEY){
        EVP_PKEY_free(sc->ptr);
        return SUCCESS;
    }else{
        return NOOP;
    }
}

Secure *Secure_PubKey(MemCh *m, Str *fname, Access *access){
    Secure *sc = (Secure *)MemCh_Alloc(m, sizeof(Secure));
    sc->type.of = TYPE_SECURE_PUBKEY;
    File file;
    File_Init(&file, fname, access, NULL);
    file.abs = File_GetAbsPath(m, file.path);
    FILE *fp = File_GetFILE(m, &file, access);
    if(fp == NULL){
        Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "unable to open file for pubkey");
        return NULL;
    }
    sc->ptr = (void *)PEM_read_PUBKEY(fp, (EVP_PKEY **)&sc->ptr, NULL, NULL);
    if(sc->ptr == NULL){
        Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "unable to interpret pubkey");
        return NULL;
    }
    return sc;
}

Secure *Secure_Key(MemCh *m, Str *fname, Access *access){
    Secure *sc = (Secure *)MemCh_Alloc(m, sizeof(Secure));
    sc->type.of = TYPE_SECURE_PRIVKEY;
    File file;
    File_Init(&file, fname, access, NULL);
    file.abs = File_GetAbsPath(m, file.path);
    FILE *fp = File_GetFILE(m, &file, access);
    if(fp == NULL){
        Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "unable to open file for private key");
        return NULL;
    }
    sc->ptr = (void *)PEM_read_PrivateKey(fp, (EVP_PKEY **)&sc->ptr, NULL, NULL);
    if(sc->ptr == NULL){
        Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "unable to interpret private key");
        return NULL;
    }
    return sc;
}
