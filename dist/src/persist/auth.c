#include <external.h>
#include <caneka.h>

void Log_AuthFail(MemCtx *m, Auth *auth, Access *ac){
    printf("Filed login attempt\n");
    return;
}

boolean Auth_Verify(MemCtx *m, Auth *auth, String *secret, Access *ac){
    if((EncPair_Fill(m, auth->saltenc, ac) & SUCCESS) != 0){
        Sha256 sha;
        Sha256_init(&sha);
        Sha256_AddString(&sha, secret);
        Sha256_AddString(&sha, auth->saltenc->dec);
        String *digest = Sha256_Get(m, &sha);

        return String_Equals(auth->digest, digest);
    }

    return FALSE;
}

Auth *Auth_Make(MemCtx *m, String *key, String *secret, Access *ac){
    DebugStack_Push(key, key->type.of);
    Auth *auth = MemCtx_Alloc(m, sizeof(Auth));
    auth->type.of = TYPE_AUTH;

    if(key != NULL){
        String *skey = EncPair_GetKey(key, ac);
        if(skey == NULL){
            Fatal("Key for auth is NULL", TYPE_AUTH);
            DebugStack_Pop();
            return NULL;
        }

        String *salt = Crypto_RandomString(m, USER_SALT_LENGTH);

        String *enc = String_Clone(m, salt);
        Salty_Enc(m, skey, enc);
        auth->saltenc = EncPair_Make(m, key, enc, NULL, ac, USER_SALT_LENGTH);

        Sha256 sha;
        Sha256_init(&sha);
        Sha256_AddString(&sha, secret);
        Sha256_AddString(&sha, salt);
        auth->digest = Sha256_Get(m, &sha);
    }

    DebugStack_Pop();
    return auth;
}
