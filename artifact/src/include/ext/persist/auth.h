typedef struct auth {
    Type type;
    EncPair *saltenc;
    Str *digest;
} Auth;

Auth *Auth_Make(MemCh *m, Str *key, Str *secret, Access *ac);
boolean Auth_Verify(MemCh *m, Auth *auth, Str *secret, Access *ac);
void Log_AuthFail(MemCh *m, Auth *auth, Access *ac);
