#include <external.h>
#include <caneka.h>

void Log_AuthFail(MemCtx *m, Auth *auth, Access *ac){
    return;
}

boolean Auth_Verify(MemCtx *m, Auth *auth, String *secret, Access *ac){
    printf("verifying...\n");
    if((EncPair_Fill(m, auth->saltenc, ac) & SUCCESS) != 0){
        /*
        Sha256 sha;
        Sha256_init(&sha);
        Sha256_AddString(&sha, secret);
        Sha256_AddString(&sha, auth->saltenc->dec);
        String *digest = Sha256_Get(m, &sha);
        Debug_Print((void *)secret, 0, "Secret: ", COLOR_PURPLE, TRUE);
        printf("\n");
        Debug_Print((void *)auth->saltenc->dec, 0, "Dec: ", COLOR_PURPLE, TRUE);
        printf("\n");
        Debug_Print((void *)digest, 0, "Digest: ", COLOR_PURPLE, TRUE);
        printf("\n");
        */

        String *s = String_Init(m, STRING_EXTEND);
        String_Add(m, s, secret);
        String_Add(m, s, auth->saltenc->dec);
        String *digest = String_Sha256(m, s);

        Debug_Print((void *)digest, 0, "s256: ", COLOR_PURPLE, TRUE);
        printf("\n");

        Debug_Print((void *)auth->digest, 0, "vs Auth->Digest: ", COLOR_CYAN, TRUE);
        printf("\n");

        return String_Equals(auth->digest, digest);
    }

    printf("verifying...FALSE\n");
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

        /*
        String *salt = Crypto_RandomString(m, USER_SALT_LENGTH);
        */

        String *salt = String_Make(m, bytes("qerjfkdlkieurjfhdic3k83"));;


        String *enc = String_Clone(m, salt);
        Salty_Enc(m, skey, enc);
        auth->saltenc = EncPair_Make(m, key, enc, (String *)Blank_Make(m), ac, USER_SALT_LENGTH);

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

        Debug_Print((void *)secret, 0, "Secret: ", COLOR_YELLOW, TRUE);
        printf("\n");
        Debug_Print((void *)salt, 0, "Salt: ", COLOR_YELLOW, TRUE);
        printf("\n");
        Debug_Print((void *)auth->digest, 0, "Digest: ", COLOR_YELLOW, TRUE);
        printf("\n");
    }

    return auth;
}
