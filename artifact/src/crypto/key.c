#include <external.h>
#include <caneka.h>

static EVP_PKEY_CTX *pctx = NULL;

status KeyInit(MemCh *m){
    if(pctx == NULL){
        ERR_load_crypto_strings();
        OpenSSL_add_all_algorithms();
        pctx = EVP_PKEY_CTX_new_from_name(NULL, "EC", NULL);
        return SUCCESS;
    }
    return NOOP;
}

status KeyCleanup(MemCh *m){
    EVP_cleanup();
    CRYPTO_cleanup_all_ex_data();
    ERR_free_strings();
    return SUCCESS;
}

EcKey *EcKey_FromPaths(MemCh *m, Str *priv, Str *pub, Access *access){
    status r = READY;
    Str *priv_s = NULL;
    Str *pub_s = NULL;

    if(priv != NULL){
        File privFile;
        File_Init(&privFile, priv, access, NULL);
        privFile.abs = privFile.path;
        r |= File_Load(m, &privFile, access);
        priv_s = privFile.sm->dest.curs->it.value;
    }
    if(pub != NULL){
        File pubFile;
        File_Init(&pubFile, pub, access, NULL);
        pubFile.abs = pubFile.path;
        r |= File_Load(m, &pubFile, access);
        pub_s = pubFile.sm->dest.curs->it.value;
    }

    return EcKey_From(m, priv_s, pub_s);
}

EcKey *EcKey_From(MemCh *m, Str *priv, Str *pub){
    EcKey *ecKey = MemCh_Alloc(m, sizeof(EcKey));
    ecKey->type.of = TYPE_EC_KEY;

    EVP_PKEY *evpKey = EVP_PKEY_new();
    ecKey->evp = evpKey;

    if(evpKey == NULL){
        printf("initial evp error\n");
        goto error;
    }

    if(priv != NULL){
        printf("decoding priv key\n");
        Str *rawPriv = Span_Get(StrVec_FromB64(m, StrVec_From(m, priv))->p, 0);
        size_t privLen = rawPriv->length;
        byte *privBytes = rawPriv->bytes;
        if(!EVP_PKEY_get_raw_private_key(evpKey, privBytes, &privLen)){
                printf("priv key evp error\n");
                goto error;
        }else{
            printf("key success\n");
        }
    }
    size_t pubLen = pub->length;
    if(pub != NULL){
        Str *rawPub = Span_Get(StrVec_FromB64(m, StrVec_From(m, pub))->p, 0);
        size_t pubLen = rawPub->length;
        byte *pubBytes = rawPub->bytes;
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

status EcKey_Free(MemCh *m, EcKey *key){
    EVP_PKEY_free(key->evp);
    return SUCCESS;
}
