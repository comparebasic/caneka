#include <external.h>
#include <caneka.h>

Span *User_Open(MemCtx *m, IoCtx *userCtx, String *id, String *pass, Access *ac){
    Debug_Print((void *)id, 0, "Making user: ", COLOR_PURPLE, TRUE);
    printf("\n");
    
    
    String *skey = EncPair_GetKey(Cont(m, bytes("test")), ac);

    Debug_Print((void *)skey, 0, "have key: ", COLOR_PURPLE, TRUE);
    printf("\n");

    IoCtx ctx;
    IoCtx_Init(m, &ctx, id, NULL, userCtx);
    if((IoCtx_Load(m, &ctx) & MISS) == 0 && ctx.files != NULL){
        File *uf = (File *)Table_Get(ctx.files, (Abstract *)String_Make(m, bytes("user")));
        Span *u = as(Abs_FromOset(m, uf->data), TYPE_SPAN); 
        Debug_Print((void *)u, 0, "User Found: ", COLOR_PURPLE, TRUE);
        printf("\n");
    }else{
        /*
        Span *u = Span_Make(m, TYPE_TABLE);
        Table_Set(u, (Abstract *)String_Make(m, bytes("id")), (Abstract *)id);
        String *random = Crypto_RandomString(m, USER_SALT_LENGTH);
        String *salt = String_Clone(m, random);
        Salty_Enc(m, skey, salt);
        Table_Set(u, (Abstract *)String_Make(m, bytes("salt")), (Abstract *)salt);
        / * TODO: make sha256 update so that the string is never stored * /
        String *prePass = String_Clone(m, id);
        String_Add(m, prePass, random);
        String *pwhash = String_Sha256(m, prePass);
        return u;
        */
        return NULL;
    }

    return NULL;
}
