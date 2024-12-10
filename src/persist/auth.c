#include <external.h>
#include <caneka.h>

void Log_AuthFail(MemCtx *m, Auth *auth, Access *ac){
    return;
}

boolean Auth_Verify(MemCtx *m, Auth *auth, String *secret, Access *ac){
    if((EncPair_Fill(m, auth->saltenc, ac) & SUCCESS) != 0){
        /*
        Sha256 sha;
        Sha256_init(&sha);
        Sha256_AddString(&sha, secret);
        Sha256_AddString(&sha, auth->saltenc->dec);
        String *digest = Sha256_Get(m, &sha);
        */

        String *s = String_Init(m, STRING_EXTEND);
        String_Add(m, s, secret);
        String_Add(m, s, auth->saltenc->dec);
        String *digest = String_Sha256(m, s);

        return String_Equals(auth->digest, digest);
    }

    return FALSE;
}

Auth *Auth_Make(MemCtx *m, String *key, String *secret, Access *ac){
    Auth *auth = MemCtx_Alloc(m, sizeof(Auth));
    auth->type.of = TYPE_AUTH;

    if(key != NULL){
        String *skey = EncPair_GetKey(key, ac);
        if(skey == NULL){
            return NULL;
        }

        String *salt = Crypto_RandomString(m, USER_SALT_LENGTH);

        String *enc = String_Clone(m, salt);
        Salty_Enc(m, skey, enc);
        auth->saltenc = EncPair_Make(m, key, enc, NULL, ac, USER_SALT_LENGTH);

        /*
        Sha256 sha;
        Sha256_init(&sha);
        Sha256_AddString(&sha, secret);
        Sha256_AddString(&sha, salt);
        auth->digest = Sha256_Get(m, &sha);
        */

        String *s = String_Init(m, STRING_EXTEND);
        String_Add(m, s, secret);
        String_Add(m, s, salt);

        auth->digest = String_Sha256(m, s);

    }

    return auth;
}
