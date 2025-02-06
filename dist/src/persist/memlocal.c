#include <external.h>
#include <caneka.h>

Chain *MemLocalToChain = NULL;
Chain *MemLocalFromChain = NULL;

static status MemLocal_addTo(MemCtx *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_STRING, (void *)String_ToLocal);
    r |= Lookup_Add(m, lk, TYPE_SPAN, (void *)Span_ToLocal);
    r |= Lookup_Add(m, lk, TYPE_WRAPPED, (void *)Single_ToLocal);
    r |= Lookup_Add(m, lk, TYPE_HASHED, (void *)Hashed_ToLocal);
    return r;
}

static status MemLocal_addFrom(MemCtx *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_STRING, (void *)String_FromLocal);
    r |= Lookup_Add(m, lk, TYPE_SPAN, (void *)Span_FromLocal);
    r |= Lookup_Add(m, lk, TYPE_WRAPPED, (void *)Single_FromLocal);
    r |= Lookup_Add(m, lk, TYPE_HASHED, (void *)Hashed_FromLocal);
    return r;
}

status MemLocal_GetLocal(MemCtx *m, void *addr, LocalPtr *lptr){
    memset(lptr, 0, sizeof(LocalPtr));
    MemSlab *sl = MemCtx_GetSlab(m, addr);
    if(sl != NULL){
        lptr->slabIdx = sl->idx;
        lptr->offset = ((void *)addr - (void *)sl->bytes);
        return SUCCESS;
    }else{
        Fatal("Slab not found, addr outside this memory context\n", TYPE_MEMLOCAL);
    }
    return NOOP;
}

Abstract *MemLocal_GetPtr(MemCtx *m, LocalPtr *lptr){
    MemSlab *sl = m->start_sl;
    while(sl != NULL){
        if(sl->idx == lptr->slabIdx){
            return (Abstract *)(sl->bytes+lptr->offset);
        }
        sl = sl->next;
    }
    return NULL;
}

Span *MemLocal_Load(MemCtx *m, String *path, IoCtx *ctx){
    status r = READY;
    Iter it;

    MemCtx *ml = MemCtx_Make();
    String *fpath = IoCtx_GetPath(m, ctx, path);

    String *idxPath = String_Clone(m, fpath);
    String_AddBytes(m, idxPath, bytes("/"), 1);
    String_AddBytes(m, idxPath, bytes(INDEX_PATHNAME), strlen(INDEX_PATHNAME));
    File *indexFile = File_Make(m, idxPath, NULL, NULL);
    indexFile->abs = indexFile->path;
    r |= File_Load(m, indexFile, NULL);
    if((r & ERROR) != 0){
        Fatal("Unable to load memlocal indexFile", TYPE_MEMLOCAL);
    }
    Span *index = (Span *)as(Abs_FromOset(m, indexFile->data), TYPE_TABLE);

    Span *slabFnames = Span_Make(m, TYPE_SPAN);
    slabFnames->type.state |= SPAN_ORDERED;
    r |= Dir_Gather(m, path, slabFnames);
    Iter_Init(&it, slabFnames);
    File slabFile;
    while((Iter_Next(&it) & END) == 0){
        String *fname = (String *)Iter_Get(&it);
        if(fname != NULL 
                && asIfc(fname, TYPE_STRING) 
                && !String_EqualsBytes(fname, bytes(INDEX_PATHNAME))){
            File_Init(&slabFile, fname, NULL, NULL);
            r |= File_Load(m, &slabFile, NULL);
            if((r & ERROR) != 0){
                Fatal("Unable to load memlocal slabs", TYPE_MEMLOCAL);
            }
            MemSlab_Attach(ml, (MemSlab *)String_ToChars(m, slabFile.data));
        }
    }

    Iter_Init(&it, index);
    while((Iter_Next(&it) & END) == 0){
        MemLocalItem *item = as(Iter_Get(&it), TYPE_MEMLOCAL_ITEM);
        if(item != NULL){
            DoFunc func = Chain_Get(MemLocalFromChain, item->typeOf);
            if(func == NULL){
                Fatal("Unable to find conversion to MemLocal Abstract", TYPE_MEMLOCAL);
                return NULL;
            }
            r |= func(ml, item->a); 
            if((r & ERROR) != 0){
                break;
            }
        }
    }

    return as(ml->start_sl->bytes, TYPE_TABLE);
}

status MemLocal_Destroy(MemCtx *m, String *path, IoCtx *ctx){
    return Dir_Destroy(m, IoCtx_GetPath(m, ctx, path));
}

status MemLocal_Persist(MemCtx *m, Span *tbl, String *path, IoCtx *ctx){
    status r = READY;

    Iter it;
    Iter_Init(&it, tbl);
    while((Iter_Next(&it) & END) == 0){
        Abstract *a = Iter_Get(&it);
        if(a != NULL){
            DoFunc func = Chain_Get(MemLocalToChain, a->type.of);
            if(func == NULL){
                Fatal("Unable to find conversion to MemLocal Abstract", TYPE_MEMLOCAL);
                return ERROR;
            }
            r |= func(m, a); 
            if((r & ERROR) != 0){
                break;
            }
        }
    }

    String *fname = IoCtx_GetPath(m, ctx, path);
    char *path_cstr = String_ToChars(m, IoCtx_GetPath(m, ctx, path));
    DIR* dir = opendir(path_cstr);
    if(dir){
        closedir(dir);
    }else if(ENOENT == errno){
        if(mkdir(path_cstr, PERMS) != 0){
            Fatal("Unable to make dir", TYPE_IOCTX);
            return ERROR;
        }
    }

    String_AddBytes(m, fname, bytes("/memslab."), strlen("/memslab."));
    i64 l = fname->length;

    File f;
    MemSlab *sl = m->start_sl;
    while(sl != NULL){
        File_Init(&f, fname, ctx->access, NULL);
        fname->length = l;
        String_Add(m, fname, String_FromInt(m, sl->idx));

        File_Init(&f, fname, ctx->access, NULL);
        f.abs = IoCtx_GetPath(m, ctx, fname);
        f.data = String_Init(m, sizeof(MemSlab));

        String_AddBytes(m, f.data, (byte *)sl, sizeof(MemSlab));
        f.type.state |= FILE_UPDATED;

        r |= File_Persist(m, &f);
        sl = sl->next;
    }

    return r;
}

status MemLocal_Init(MemCtx *m){
    if(MemLocalToChain == NULL){
        Lookup *funcs = Lookup_Make(m, _TYPE_START, MemLocal_addTo, NULL);
        MemLocalToChain = Chain_Make(m, funcs);

        funcs = Lookup_Make(m, _TYPE_START, MemLocal_addFrom, NULL);
        MemLocalFromChain = Chain_Make(m, funcs);
        return SUCCESS;
    }

    return NOOP;
}

Span *MemLocal_Make(){
    MemCtx *m = MemCtx_Make();
    return Span_Make(m, TYPE_TABLE);
}
