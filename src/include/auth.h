typedef struct auth {
    Type type;
    String *key;
    EncPair *saltenc;
    String *digest;
} Auth;

Auth *Auth_Make(MemCtx *m, String *key, String *secret, Access *ac);
