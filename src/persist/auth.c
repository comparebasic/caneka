#include <external.h>
#include <caneka.h>

void Log_AuthFail(MemCtx *m, Auth *auth, Access *ac){
    return;
}

boolean Auth_Verify(MemCtx *m, Auth *auth, String *secret, Access *ac){
    if((EncPair_Fill(m, auth->saltenc, ac) & SUCCESS) != 0){
        Sha256 sha;
        Sha256_init(&sha);
        Sha256_AddString(&sha, secret);
        Sha256_AddString(&sha, auth->saltenc->dec);
        return String_Equals(auth->digest, Sha256_Get(m, &sha));
    }

    return FALSE;
}

Auth *Auth_Make(MemCtx *m, String *key, String *secret, Access *ac){
    Auth *auth = MemCtx_Alloc(m, sizeof(Auth));
    auth->type.of = TYPE_AUTH;

    String *skey = EncPair_GetKey(key, ac);
    if(skey == NULL){
        return NULL;
    }

    String *salt = Crypto_RandomString(m, USER_SALT_LENGTH);
    String *enc = String_Clone(m, salt);
    Salty_Enc(m, skey, enc);
    auth->saltenc = EncPair_Make(m, key, enc, (String *)Blank_Make(m), ac);

    Sha256 sha;
    Sha256_init(&sha);
    Sha256_AddString(&sha, secret);
    Sha256_AddString(&sha, salt);
    auth->digest = Sha256_Get(m, &sha);

    return auth;
}
