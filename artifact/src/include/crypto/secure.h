typedef struct secure {
    Type type;
    void *ptr;
} Secure;

status Secure_Free(MemCh *m, Secure *sc);
Secure *Secure_PubKey(MemCh *m, Str *pubKey, Access *access);
Secure *Secure_Key(MemCh *m, Str *pubKey, Access *access);
