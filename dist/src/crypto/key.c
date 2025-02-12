#include <external.h>
#include <caneka.h>

static EVP_PKEY_CTX *pctx = NULL;

status KeyInit(MemCtx *m){
    if(pctx == NULL){
        pctx = EVP_PKEY_CTX_new_from_name(NULL, "EC", NULL);
        return SUCCESS;
    }
    return NOOP;
}

EcKey *EcKey_FromPaths(MemCtx *m, String *priv, String *pub, Access *access){
    status r = READY;
    String *priv_s = NULL;
    String *pub_s = NULL;

    if(priv != NULL){
        File privFile;
        File_Init(&privFile, priv, access, NULL);
        privFile.abs = privFile.path;
        r |= File_Load(m, &privFile, access);
        priv_s = privFile.data;
    }
    if(pub != NULL){
        File pubFile;
        File_Init(&pubFile, pub, access, NULL);
        pubFile.abs = pubFile.path;
        r |= File_Load(m, &pubFile, access);
        pub_s = pubFile.data;
    }

    return EcKey_From(m, priv_s, pub_s);
}

EcKey *EcKey_From(MemCtx *m, String *priv, String *pub){
    EcKey *ecKey = MemCtx_Alloc(m, sizeof(EcKey));
    ecKey->type.of = TYPE_EC_KEY;

    EVP_PKEY *evpKey = EVP_PKEY_new();
    ecKey->evp = evpKey;

    TrackMalloc(_EVP_PKEY_SIZE, TYPE_EC_KEY);
    if(evpKey == NULL){
       goto error;
    }

    size_t privLen = String_Length(priv);
    if(priv != NULL && !EVP_PKEY_get_raw_private_key(evpKey, 
            bytes(String_ToChars(m, priv)), &privLen)){
       goto error;
    }
    size_t pubLen = String_Length(pub);
    if(pub != NULL && !EVP_PKEY_get_raw_public_key(evpKey, 
           bytes(String_ToChars(m, pub)), &pubLen)){
       goto error;
    }

    return ecKey;
error:
    ecKey->type.state |= ERROR;
    return ecKey;
}

status EcKey_Free(MemCtx *m, EcKey *key){
    EVP_PKEY_free(key->evp);
    TrackFree(NULL, _EVP_PKEY_SIZE);
    return SUCCESS;
}
