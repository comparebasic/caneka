#include <external.h>
#include <caneka.h>

Span *User_Open(MemCtx *m, IoCtx *userCtx, String *id, String *pass, Access *ac, Table *data){
    Debug_Print((void *)id, 0, "Making user: ", COLOR_PURPLE, TRUE);
    printf("\n");
    
    String *skey = EncPair_GetKey(Cont(m, bytes("test")), ac);

    IoCtx ctx;
    IoCtx_Init(m, &ctx, id, NULL, userCtx);
    if((IoCtx_Load(m, &ctx) & MISS) == 0 && ctx.files != NULL){
        File *uf = (File *)Table_Get(ctx.files, (Abstract *)String_Make(m, bytes("user")));
        Span *u = as(Abs_FromOset(m, uf->data), TYPE_SPAN); 
        Debug_Print((void *)u, 0, "User Found: ", COLOR_PURPLE, TRUE);
        printf("\n");
    }else{
        File user;
        File pwauth;
        File_Init(&pwauth, String_Make(m, "user.data"), ac, ctx);
        File_Init(&pwauth, String_Make(m, "password.auth"), ac, ctx);

        Table_Set(data, (Abstract *)String_Make(m, bytes("id")), (Abstract *)id);
        user.data = Oset_To(m, NULL, (Abstract *)data); 
        user.type.state |= FILE_UPDATED;


        return u;
    }

    return NULL;
}
