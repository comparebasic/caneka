#include <external.h>
#include <caneka.h>
status User_Delete(MemCtx *m, IoCtx *userCtx, String *id, Access *ac){
    IoCtx ctx;
    File file;
    IoCtx_Init(m, &ctx, id, NULL, userCtx);

    File_Init(&file, String_Make(m, bytes("password.auth")), ac, NULL);
    file.abs = IoCtx_GetPath(m, &ctx, file.path);
    File_Delete(&file);

    File_Init(&file, String_Make(m, bytes("user.data")), ac, NULL);
    file.abs = IoCtx_GetPath(m, &ctx, file.path);
    File_Delete(&file);

    return IoCtx_Destroy(m, &ctx, ac);
}

Span *User_Open(MemCtx *m, IoCtx *userCtx, String *id, String *secret, Access *ac, Span *data){
    if(DEBUG_USER){
        Debug_Print((void *)id, 0, "Making user: ", DEBUG_USER, TRUE);
        printf("\n");
    }

    String *key = Cont(m, bytes("test"));
    String *skey = EncPair_GetKey(key, ac);

    IoCtx ctx;
    IoCtx_Init(m, &ctx, id, NULL, userCtx);
    if((IoCtx_Load(m, &ctx) & NOOP) == 0 && ctx.files != NULL){
        if(DEBUG_USER){
            Debug_Print((void *)id, 0, "Found: ", DEBUG_USER, TRUE);
            printf("\n");
        }
        File *pwauth = (File *)Table_Get(ctx.files, (Abstract *)String_Make(m, bytes("password.auth")));
        if(pwauth == NULL){
            Fatal("pwauth file not found for ser", TYPE_AUTH);
        }
        pwauth->abs = IoCtx_GetPath(m, &ctx, pwauth->path);
        File_Load(m, pwauth, ac);

        Auth *auth = as(Abs_FromOset(m, pwauth->data), TYPE_AUTH);
        if(!Auth_Verify(m, auth, secret, ac)){
            Log_AuthFail(m, auth, ac);
            return NULL;
        }

        File *udata = (File *)Table_Get(ctx.files, (Abstract *)String_Make(m, bytes("user.data")));
        udata->abs = IoCtx_GetPath(m, &ctx, udata->path);
        File_Load(m, udata, ac);
        Span *udataTbl = (Span *)Abs_FromOset(m, udata->data);
        if(data != NULL){
            Table_Merge(udataTbl, data);
            udata->data = Oset_To(m, NULL, (Abstract *)udataTbl);
            udata->type.state |= FILE_UPDATED;
        }

        IoCtx_Persist(m, &ctx); 
        return udataTbl;
    }else{
        if(DEBUG_USER){
            Debug_Print((void *)id, 0, "Making new: ", DEBUG_USER, TRUE);
            printf("\n");
        }
        File pwauth;
        File_Init(&pwauth, String_Make(m, bytes("password.auth")), ac, &ctx);
        pwauth.data = Oset_To(m, NULL, (Abstract *)Auth_Make(m, key, secret, ac));
        pwauth.type.state |= FILE_UPDATED;

        /*
        Debug_Print((void *)pwauth.data, 0, "PwAuthOset: ", COLOR_PURPLE, TRUE);
        printf("\n");
        */

        File user;
        File_Init(&user, String_Make(m, bytes("user.data")), ac, &ctx);

        if(data == NULL){
            data = Span_Make(m, TYPE_TABLE);
        }
        Table_Set(data, (Abstract *)String_Make(m, bytes("id")), (Abstract *)id);
        user.data = Oset_To(m, NULL, (Abstract *)data); 
        user.type.state |= FILE_UPDATED;

        if(DEBUG_USER){
            Debug_Print((void *)ctx.files, 0, "persisting?", DEBUG_USER, TRUE);
            printf("\n");
        }
        
        IoCtx_Persist(m, &ctx); 
        return data;
    }

    return NULL;
}
