#include <external.h>
#include <caneka.h>

static status ioCtx_LoadFileList(MemCtx *m, IoCtx *ctx){
    File index;
    File_Init(&index, IoCtx_GetIndexName(m, ctx), ctx->access, NULL);
    index.abs = IoCtx_GetPath(m, ctx, index.path);
    File_Load(m, &index, ctx->access);
    if((index.type.state & FILE_LOADED) != 0){
        ctx->files = (Span *)Abs_FromOset(m, index.data);
        if(ctx->files != NULL && ctx->files->type.of == TYPE_TABLE){
            return SUCCESS;
        }
    }

    return NOOP;
}

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
        if(ctx->prior != NULL && ctx->prior->abs != NULL){
            String_Add(m, s, ctx->prior->abs);
            String_AddBytes(m, s, bytes("/"), 1);
            String_Add(m, s, ctx->root);
            ctx->abs = s;
        }else{
            ctx->abs = pathRecurse(m, s, ctx, ctx->prior);
        }
    }
    return ctx->abs;
}

String *IoCtx_GetIndexName(MemCtx *m, IoCtx *ctx){
    return String_Make(m, bytes("index"));
}

String *IoCtx_GetMStorePath(MemCtx *m, IoCtx *ctx){
    return IoCtx_GetPath(m, ctx, IoCtx_GetMStoreName(m, ctx));
}

String *IoCtx_GetMStoreName(MemCtx *m, IoCtx *ctx){
    return String_Make(m, bytes("mstore"));
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

status IoCtx_LoadOrReserve(MemCtx *m, IoCtx *ctx){
    status r = READY;
    String *s = String_Init(m, STRING_EXTEND);
    String *path = IoCtx_GetAbs(m, ctx);

    char *path_cstr = String_ToChars(m, path);
    DIR* dir = opendir(path_cstr);
    if(dir){
        closedir(dir);
    }else if(ENOENT == errno){
        if(mkdir(path_cstr, PERMS) != 0){
            printf("%s: %s\n", path_cstr, strerror(errno));
            Fatal("Unable to make dir", TYPE_IOCTX);
            return ERROR;
        }
    }

    return SUCCESS;
}

status IoCtx_Load(MemCtx *m, IoCtx *ctx){
    status r = READY;
    String *s = String_Init(m, STRING_EXTEND);
    String *path = IoCtx_GetAbs(m, ctx); 

    char *path_cstr = String_ToChars(m, path);
    DIR* dir = opendir(path_cstr);
    if(dir == NULL && ENOENT == errno){
        return NOOP;
    }
    closedir(dir);

    ctx->mstore = MemLocal_Load(m, ctx);
    ioCtx_LoadFileList(m, ctx);

    return SUCCESS;
}

status IoCtx_Persist(MemCtx *m, IoCtx *ctx){
    status r = READY;

    r |= IoCtx_LoadOrReserve(m, ctx);

    Iter it;
    Iter_Init(&it, ctx->files);
    
    while((Iter_Next(&it) & END) == 0){
        Hashed *h = (Hashed *)Iter_Get(&it);
        if(h != NULL){
            File *file = (File *)h->value;
            file->abs = IoCtx_GetPath(m, ctx, file->path);
            r |= File_Persist(m, file);
        }
    }

    r |= MemLocal_Persist(m, ctx->mstore, ctx);
    String *os = Oset_To(m, String_Make(m, bytes("index")), (Abstract *)ctx->files);
    File *index = File_Make(m, IoCtx_GetIndexName(m, ctx), NULL, NULL); 
    index->abs = IoCtx_GetPath(m, ctx, index->path);
    index->data = os;
    index->type.state |= FILE_UPDATED;
    File_Persist(m, index);

    return r;
}

status IoCtx_Destroy(MemCtx *m, IoCtx *ctx, Access *access){
    String *abs = IoCtx_GetAbs(m, ctx); 
    char *abs_cstr = String_ToChars(m, abs);
    /* TODO: remove files ... */
    Iter it;
    Iter_Init(&it, ctx->files);
    while((Iter_Next(&it) & END) == 0){
        Hashed *h = (Hashed *)Iter_Get(&it);
        if(h != NULL){
            File *file = (File *)h->value;
            file->abs = IoCtx_GetPath(m, ctx, file->path);
            File_Delete(file);
        }
    }

    File file;
    File_Init(&file, IoCtx_GetIndexName(m, ctx), access, NULL);
    file.abs = IoCtx_GetPath(m, ctx, file.path);
    File_Delete(&file);

    MemLocal_Destroy(m, ctx);

    /* remove dir */
    if(rmdir(abs_cstr) == 0){
        return SUCCESS;
    }

    printf("IoCtx Error in Destroy\n");
    return ERROR;
}

status IoCtx_Init(MemCtx *m, IoCtx *ctx, String *root, Access *access, IoCtx *prior){
    memset(ctx, 0, sizeof(IoCtx));

    ctx->m = m;
	ctx->type.of = TYPE_IOCTX;
    ctx->root = root;
    ctx->access = access;
    ctx->prior = prior;

    ctx->files = Span_Make(m, TYPE_TABLE);
    ctx->mstore = MemLocal_Make(m);

    return SUCCESS;
}

IoCtx *IoCtx_Make(MemCtx *m, String *root, Access *access, IoCtx *prior){
    if(root == NULL){
        Fatal("Error getAbsPath: missing base_s on ioctx\n", TYPE_IOCTX);
    }
	IoCtx* ctx = (IoCtx*)MemCtx_Alloc(m, sizeof(IoCtx));
    IoCtx_Init(m, ctx, root, access, prior);

    return ctx;
}

status IoCtx_Open(MemCtx *m, IoCtx *ctx, String *root, Access *access, IoCtx *prior){
    IoCtx_Init(m, ctx, root, access, prior);
    return IoCtx_Load(m, ctx);
}

