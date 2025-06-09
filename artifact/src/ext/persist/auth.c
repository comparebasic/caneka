#include <external.h>
#include <caneka.h>

void Log_AuthFail(MemCh *m, Auth *auth, Access *ac){
    printf("Filed login attempt\n");
    return;
}

boolean Auth_Verify(MemCh *m, Auth *auth, Str *secret, Access *ac){
    if((EncPair_Fill(m, auth->saltenc, ac) & SUCCESS) != 0){
        Str *s = Str_Init(m, STRING_EXTEND);
        String_Add(m, s, secret);
        String_Add(m, s, auth->saltenc->dec);
        Str *digest = Digest_Sha256(m, s);

        return String_Equals(auth->digest, digest);
    }

    return FALSE;
}

Auth *Auth_Make(MemCh *m, Str *key, Str *secret, Access *ac){
    DebugStack_Push(key, key->type.of);
    Auth *auth = MemCh_Alloc(m, sizeof(Auth));
    auth->type.of = TYPE_AUTH;

    if(key != NULL){
        Str *skey = EncPair_GetKey(m, key, ac);
        if(skey == NULL){
            Fatal("Key for auth is NULL", TYPE_AUTH);
            DebugStack_Pop();
            return NULL;
        }

        Str *salt = Crypto_RandomString(m, USER_SALT_LENGTH);

        Str *enc = String_Clone(m, salt);
        Salty_Enc(m, skey, enc);
        auth->saltenc = EncPair_Make(m, key, enc, NULL, ac, USER_SALT_LENGTH);

        Str *s = String_Init(m, STRING_EXTEND);
        String_Add(m, s, secret);
        String_Add(m, s, salt);
        auth->digest = Digest_Sha256(m, s);
    }

    DebugStack_Pop();
    return auth;
}
