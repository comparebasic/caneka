typedef struct auth {
    Type type;
    EncPair *saltenc;
    String *digest;
} Auth;

Auth *Auth_Make(MemCtx *m, String *key, String *secret, Access *ac);
boolean Auth_Verify(MemCtx *m, Auth *auth, String *secret, Access *ac);
void Log_AuthFail(MemCtx *m, Auth *auth, Access *ac);
