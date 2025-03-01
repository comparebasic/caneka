typedef struct secure {
    Type type;
    void *ptr;
} Secure;

status Secure_Free(MemCtx *m, Secure *sc);
Secure *Secure_PubKey(MemCtx *m, String *pubKey, struct access *access);
Secure *Secure_Key(MemCtx *m, String *pubKey, struct access *access);
