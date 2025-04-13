#include <external.h>
#include <caneka.h>

static status ioCtx_LoadFileList(MemCh *m, IoCtx *ctx){
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

static String *pathRecurse(MemCh *m, String *s, IoCtx *ctx, IoCtx *prior){
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


String *IoCtx_GetAbs(MemCh *m, IoCtx *ctx){
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

String *IoCtx_GetIndexName(MemCh *m, IoCtx *ctx){
    return String_Make(m, bytes("index"));
}

String *IoCtx_GetMStoreName(MemCh *m, IoCtx *ctx){
    return String_Make(m, bytes("mstore"));
}

String *IoCtx_GetMStorePath(MemCh *m, IoCtx *ctx){
    return IoCtx_GetPath(m, ctx, IoCtx_GetMStoreName(m, ctx));
}

String *IoCtx_GetPath(MemCh *m, IoCtx *ctx, String *path){
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

status IoCtx_LoadOrReserve(MemCh *m, IoCtx *ctx){
    status r = READY;
    String *s = String_Init(m, STRING_EXTEND);
    String *path = IoCtx_GetAbs(m, ctx);

    char *path_cstr = String_ToChars(m, path);
    DIR* dir = opendir(path_cstr);
    if(dir){
        closedir(dir);
        return NOOP;
    }else if(ENOENT == errno){
        if(mkdir(path_cstr, PERMS) != 0){
            printf("%s: %s\n", path_cstr, strerror(errno));
            Fatal("Unable to make dir", TYPE_IOCTX);
            return ERROR;
        }
        return SUCCESS;
    }
    return NOOP;
}

status IoCtx_Load(MemCh *m, IoCtx *ctx){
    DebugStack_Push(ctx->root, ctx->root->type.of);
    status r = READY;
    String *s = String_Init(m, STRING_EXTEND);
    String *path = IoCtx_GetAbs(m, ctx); 
    DebugStack_SetRef(path, path->type.of);

    char *path_cstr = String_ToChars(m, path);
    DIR* dir = opendir(path_cstr);
    if(dir == NULL && ENOENT == errno){
        DebugStack_Pop();
        return NOOP;
    }
    closedir(dir);

    ctx->mstore = MemLocal_Load(m, IoCtx_GetMStorePath(m, ctx), NULL);
    ioCtx_LoadFileList(m, ctx);

    DebugStack_Pop();
    return SUCCESS;
}

status IoCtx_Persist(MemCh *m, IoCtx *ctx){
    String *abs = IoCtx_GetAbs(m, ctx);
    DebugStack_Push(abs, abs->type.of);
    status r = READY;

    r |= IoCtx_LoadOrReserve(m, ctx);
    if(r & ERROR){
        printf("Error making dir\n");
        return r;
    }

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

    String *os = Oset_To(m, NULL, (Abstract *)ctx->files);
    if(DEBUG_IOCTX){
        DPrint((Abstract *)os, DEBUG_IOCTX, "FileOset: ");
    }
    File *index = File_Make(m, IoCtx_GetIndexName(m, ctx), NULL, NULL); 
    index->abs = IoCtx_GetPath(m, ctx, index->path);
    index->data = os;
    index->type.state |= FILE_UPDATED;
    File_Persist(m, index);

    r |= MemLocal_Persist(ctx->m, ctx->mstore, IoCtx_GetMStorePath(m, ctx), NULL);

    DebugStack_Pop();
    return r;
}

status IoCtx_Destroy(MemCh *m, IoCtx *ctx, Access *access){
    status r = READY;
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

    String *memLocalPath = IoCtx_GetPath(m, ctx, IoCtx_GetMStoreName(m, ctx));
    r |= MemLocal_Destroy(m, memLocalPath, NULL);

    /* remove dir */
    if(rmdir(abs_cstr) == 0){
        return r|SUCCESS;
    }

    printf("IoCtx Error in Destroy\n");
    return r|ERROR;
}

status IoCtx_Init(MemCh *m, IoCtx *ctx, String *root, Access *access, IoCtx *prior){
    memset(ctx, 0, sizeof(IoCtx));

    ctx->m = m;
	ctx->type.of = TYPE_IOCTX;
    ctx->root = root;
    ctx->access = access;
    ctx->prior = prior;
    ctx->files = Span_Make(m);

    return SUCCESS;
}

IoCtx *IoCtx_Make(MemCh *m, String *root, Access *access, IoCtx *prior){
    if(root == NULL){
        Fatal("Error getAbsPath: missing base_s on ioctx\n", TYPE_IOCTX);
    }
	IoCtx* ctx = (IoCtx*)MemCh_Alloc(m, sizeof(IoCtx));
    IoCtx_Init(m, ctx, root, access, prior);
    ctx->mstore = MemLocal_Make(TYPE_TABLE);

    return ctx;
}

status IoCtx_Open(MemCh *m, IoCtx *ctx, String *root, Access *access, IoCtx *prior){
    DebugStack_Push("IoCtx_Open", TYPE_CSTR);
    IoCtx_Init(m, ctx, root, access, prior);
    status r = IoCtx_Load(m, ctx);
    DebugStack_Pop();
    return r;
}

