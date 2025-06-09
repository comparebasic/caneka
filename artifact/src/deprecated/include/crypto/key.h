typedef struct ec_key {
    Type type;
    void *evp;
} EcKey;

#define _EVP_PKEY_SIZE 731

status KeyInit(MemCh *m);
EcKey *EcKey_FromPaths(MemCh *m, Str *priv, Str *pub, struct access *access);
EcKey *EcKey_From(MemCh *m, Str *priv, Str *pub);
status EcKey_Free(MemCh *m, EcKey *key);
