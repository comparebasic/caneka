#include <external.h>
#include <caneka.h>

static EVP_PKEY_CTX *pctx = NULL;

status KeyInit(MemCtx *m){
    if(pctx == NULL){
        ERR_load_crypto_strings();
        OpenSSL_add_all_algorithms();
        pctx = EVP_PKEY_CTX_new_from_name(NULL, "EC", NULL);
        return SUCCESS;
    }
    return NOOP;
}

status KeyCleanup(MemCtx *m){
    EVP_cleanup();
    CRYPTO_cleanup_all_ex_data();
    ERR_free_strings();
    return SUCCESS;
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
        printf("initial evp error\n");
        goto error;
    }

    DPrint((void *)priv, COLOR_PURPLE, "privKey: ");
    DPrint((void *)pub, COLOR_PURPLE, "pubKey: ");
    if(priv != NULL){
        printf("decoding priv key\n");
        String *rawPriv = String_FromB64(m, priv);
        size_t privLen = String_Length(rawPriv);
        byte *privBytes = bytes(String_ToChars(m, rawPriv));
        if(!EVP_PKEY_get_raw_private_key(evpKey, privBytes, &privLen)){
                printf("priv key evp error\n");
                goto error;
        }else{
            printf("key success\n");
        }
    }
    size_t pubLen = String_Length(pub);
    if(pub != NULL){
        printf("decoding pub key\n");
        String *rawPub = String_FromB64(m, pub);
        size_t pubLen = String_Length(rawPub);
        byte *pubBytes = bytes(String_ToChars(m, rawPub));
        if(!EVP_PKEY_get_raw_public_key(evpKey, 
               pubBytes, &pubLen)){
            printf("pub key evp error\n");
            goto error;
        }
    }

    return ecKey;
error:
    ERR_print_errors_fp(stderr);
    ecKey->type.state |= ERROR;
    return ecKey;
}

status EcKey_Free(MemCtx *m, EcKey *key){
    EVP_PKEY_free(key->evp);
    TrackFree(NULL, _EVP_PKEY_SIZE);
    return SUCCESS;
}
