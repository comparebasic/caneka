#include <external.h>
#include <caneka.h>

static String *pathRecurse(MemCtx *m, String *s, IoCtx *ctx, IoCtx *prior){
    if(prior->prior != NULL){
        String_Add(m, s, pathRecurse(m, s, prior, prior->prior));
    }else{
        String_Add(m, s, prior->root);
        String_AddBytes(m, s, bytes("/"), 1);
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
        String_AddBytes(m, s, bytes("/"), 1);
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
        r |= File_Persist(file);
    }

    r |= MemKeyed_Persist(m, ctx);

    return r;
}

IoCtx *IoCtx_MakeRoot(MemCtx *m, String *root, Access *access){
    char buff[PATH_BUFFLEN];
    char *path = getcwd(buff, PATH_BUFFLEN);
    if(path != NULL){
        int l = strlen(path);
        int total = l+root->length+1;
        if(total > PATH_BUFFLEN){
            Fatal("MakeRoot path too long", TYPE_IOCTX);
        }
        buff[l] = '/';
        memcpy(buff+l+1, root->bytes, root->length);
        buff[total] = '\0';
        return IoCtx_Make(m, String_Make(m, bytes(buff)), access, NULL);
    }else{
        Fatal("Unable to get Current Working Directory", TYPE_IOCTX);
        return NULL;
    }
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
