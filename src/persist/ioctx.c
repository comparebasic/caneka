#include <external.h>
#include <caneka.h>

static String *pathRecurse(MemCtx *m, String *s, IoCtx *ctx, IoCtx *prior){
    if(prior != NULL && prior->prior != NULL){
        String_Add(m, s, pathRecurse(m, s, prior, prior->prior));
    }else{
        if(prior != NULL){
            if(s->length > 0){
                String_AddBytes(m, s, bytes("/"), 1);
            }
            String_Add(m, s, prior->root);
        }
        if(s->length > 0){
            String_AddBytes(m, s, bytes("/"), 1);
        }
        String_Add(m, s, ctx->root);
    }
    return s;
}

String *IoCtx_GetAbs(MemCtx *m, IoCtx *ctx){
    if(ctx->abs == NULL){
        String *s = String_Init(m, STRING_EXTEND);
        ctx->abs = pathRecurse(m, s, ctx, ctx->prior);
    }
    return ctx->abs;
}

String *IoCtx_GetPath(MemCtx *m, IoCtx *ctx, String *path){
    String *s = String_Init(m, STRING_EXTEND);
    String *abs = IoCtx_GetAbs(m, ctx); 
    String_Add(m, s, abs);
    if(path != NULL){
        if(s->length > 0){
            String_AddBytes(m, s, bytes("/"), 1);
        }
        String_Add(m, s, path);
    }

	return s;
}

status IoCtx_Persist(MemCtx *m, IoCtx *ctx){
    status r = READY;
    String *s = String_Init(m, STRING_EXTEND);
    String *path = pathRecurse(m, s, ctx, ctx->prior);

    char *path_cstr = String_ToChars(m, path);
    DIR* dir = opendir(path_cstr);
    if(dir){
        closedir(dir);
    }else if(ENOENT == errno){
        if(mkdir(path_cstr, PERMS) != 0){
            Fatal("Unable to make dir", TYPE_IOCTX);
            return ERROR;
        }
    }

    while(Iter_Next(ctx->it) != END){
        File *file = (File *)Iter_Get(ctx->it);
        r |= File_Persist(m, file);
    }

    r |= MemKeyed_Persist(m, ctx);

    return r;
}

status IoCtx_Destroy(MemCtx *m, IoCtx *ctx, Access *access){
    String *abs = IoCtx_GetAbs(m, ctx); 
    char *abs_cstr = String_ToChars(m, abs);
    /* TODO: remove files ... */
    Iter it;
    Iter_Init(&it, ctx->files);
    while(Iter_Next(&it) != END){
        File *file = (File *)Iter_Get(&it);
        File_Delete(file);
    }

    /* remove dir */
    if(rmdir(abs_cstr) == 0){
        return SUCCESS;
    }
    return ERROR;
}

IoCtx *IoCtx_Make(MemCtx *m, String *root, Access *access, IoCtx *prior){
    if(root == NULL){
        Fatal("Error getAbsPath: missing base_s on ioctc\n", TYPE_IOCTX);
    }
	IoCtx* ctx = (IoCtx*)MemCtx_Alloc(m, sizeof(IoCtx));
	ctx->type.of = TYPE_IOCTX;
    ctx->root = root;
    ctx->access = access;
    ctx->prior = prior;

    ctx->tbl = Span_Make(m, TYPE_TABLE);
    ctx->files = Span_Make(m, TYPE_SPAN);
    ctx->m = MemKeyed_Make(m, ctx->files);
    ctx->it = Iter_Make(m, ctx->tbl);

    return ctx;
}
