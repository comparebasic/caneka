typedef struct ec_key {
    Type type;
    void *evp;
} EcKey;

#define _EVP_PKEY_SIZE 731

status KeyInit(MemCtx *m);
EcKey *EcKey_FromPaths(MemCtx *m, String *priv, String *pub, struct access *access);
EcKey *EcKey_From(MemCtx *m, String *priv, String *pub);
status EcKey_Free(MemCtx *m, EcKey *key);
