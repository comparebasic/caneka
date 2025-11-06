typedef struct auth_cred {
    Type type;
    digest salt;
    union {
        byte hash[DIGEST_SIZE];
        byte sig[SIGNATURE_SIZE];
    } val;
} AuthCred;

AuthCred *AuthCred_Make(MemCh *m);
status AuthCred_Resolve(MemCh *m, AuthCred *cred, AuthCred *target, Abstract *authentee);
