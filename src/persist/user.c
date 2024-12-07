#include <external.h>
#include <caneka.h>

Span *User_Open(MemCtx *m, IoCtx *userCtx, String *id, String *secret, Access *ac, Span *data){
    if(DEBUG_USER){
        Debug_Print((void *)id, 0, "Making user: ", COLOR_PURPLE, TRUE);
        printf("\n");
    }

    String *key = Cont(m, bytes("test"));
    String *skey = EncPair_GetKey(key, ac);

    IoCtx ctx;
    IoCtx_Init(m, &ctx, id, NULL, userCtx);
    if((IoCtx_Load(m, &ctx) & MISS) == 0 && ctx.files != NULL){
        File *udata = (File *)Table_Get(ctx.files, (Abstract *)String_Make(m, bytes("user.data")));
        File *pwauth = (File *)Table_Get(ctx.files, (Abstract *)String_Make(m, bytes("password.data")));
        File_Load(m, pwauth, ac);
        Auth *auth = as(Abs_FromOset(m, pwauth->data), TYPE_AUTH);
        if(!Auth_Verify(m, auth, secret, ac)){
            Log_AuthFail(m, auth, ac);
            return NULL;
        }
        File_Load(m, udata, ac);

        return (Span *)Abs_FromOset(m, udata->data);
    }else{
        File pwauth;
        File_Init(&pwauth, String_Make(m, bytes("password.auth")), ac, &ctx);
        pwauth.data = Oset_To(m, NULL, (Abstract *)Auth_Make(m, key, secret, ac));
        pwauth.type.state |= FILE_UPDATED;

        Debug_Print((void *)pwauth.data, 0, "Auth: ", COLOR_PURPLE, TRUE);
        printf("\n");

        File user;
        File_Init(&user, String_Make(m, bytes("user.data")), ac, &ctx);
        if(data == NULL){
            data = Span_Make(m, TYPE_TABLE);
        }
        Table_Set(data, (Abstract *)String_Make(m, bytes("id")), (Abstract *)id);
        user.data = Oset_To(m, NULL, (Abstract *)data); 
        user.type.state |= FILE_UPDATED;

        /*
        IoCtx_Persist(m, &ctx); 
        */
        return data;
    }

    return NULL;
}
