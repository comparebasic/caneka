#include <external.h>
#include <caneka.h>

void Log_AuthFail(MemCtx *m, Auth *auth, Access *ac){
    printf("Filed login attempt\n");
    return;
}

boolean Auth_Verify(MemCtx *m, Auth *auth, String *secret, Access *ac){
    if((EncPair_Fill(m, auth->saltenc, ac) & SUCCESS) != 0){
        String *s = String_Init(m, STRING_EXTEND);
        String_Add(m, s, secret);
        String_Add(m, s, auth->saltenc->dec);
        String *digest = Digest_Sha256(m, s);

        return String_Equals(auth->digest, digest);
    }

    return FALSE;
}

Auth *Auth_Make(MemCtx *m, String *key, String *secret, Access *ac){
    DebugStack_Push(key, key->type.of);
    Auth *auth = MemCtx_Alloc(m, sizeof(Auth));
    auth->type.of = TYPE_AUTH;

    if(key != NULL){
        String *skey = EncPair_GetKey(m, key, ac);
        if(skey == NULL){
            Fatal("Key for auth is NULL", TYPE_AUTH);
            DebugStack_Pop();
            return NULL;
        }

        String *salt = Crypto_RandomString(m, USER_SALT_LENGTH);

        String *enc = String_Clone(m, salt);
        Salty_Enc(m, skey, enc);
        auth->saltenc = EncPair_Make(m, key, enc, NULL, ac, USER_SALT_LENGTH);

        String *s = String_Init(m, STRING_EXTEND);
        String_Add(m, s, secret);
        String_Add(m, s, salt);
        auth->digest = Digest_Sha256(m, s);
    }

    DebugStack_Pop();
    return auth;
}
