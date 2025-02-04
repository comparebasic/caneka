#include <external.h>
#include <caneka.h>

Chain *MemLocalToChain = NULL;
Chain *MemLocalFromChain = NULL;

static status MemLocal_addTo(MemCtx *m, DoFunc from, DoFunc to){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_STRING, (void *)String_ToLocal);
    r |= Lookup_Add(m, lk, TYPE_SPAN, (void *)Span_ToLocal);
    r |= Lookup_Add(m, lk, TYPE_WRAPPED, (void *)Single_ToLocal);
    r |= Lookup_Add(m, lk, TYPE_HASHED, (void *)Hashed_ToLocal);
    return r;
}

static status MemLocal_addFrom(MemCtx *m, DoFunc from, DoFunc to){
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
            return sl->bytes+lptr->offset;
        }
        sl = sl->next;
    }
    return NULL;
}

Span *MemLocal_Load(MemCtx *m, String *path, IoCtx *ctx){
    status r = READY;

    MemCtx *ml = MemCtx_Make();
    Span *tbl = Span_Make(ml, TYPE_TABLE);

    String *fpath = IoCtx_GetPath(m, ctx, path);

    String *idxPath = String_Clone(m, fpath);
    String_AddBytes(m, idxPath, bytes("/"), 1);
    String_Add(m, idxPath, bytes(INDEX_PATHNAME), strlen(INDEX_PATHNAME));
    File *index = File_Make(m, idxPath, NULL, NULL);
    index->abs = index->path;
    r |= File_Load(m, index, NULL);
    if((r & ERROR) != 0){
        Fatal("Unable to load memlocal index", TYPE_MEMLOCAL);
    }
    Span *tbl = (Span *)as(Abs_FromOset(m, index->data), TYPE_TABLE);

    r |= Dir_Climb(m, fpath, NULL, setMl, (Abstract *ml));
    if((r & ERROR) != 0){
        Fatal("Unable to load memlocal slabs", TYPE_MEMLOCAL);
    }

    Iter it;
    Iter_Init(&it, tbl);
    while((Iter_Next(&it) & END) == 0){
        MemLocalItem *item = as(Iter_Get(&it), TYPE_MEMLOCAL_ITEM);
        if(item != NULL){
            DoFunc func = Lookup_Get(MemLocalFromChain, item->typeOf);
            if(func == NULL){
                Fatal("Unable to find conversion to MemLocal Abstract", TYPE_MEMLOCAL);
                return ERROR;
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

status MemLocal_Persist(MemCtx *m, Span *tbl, Span *path, IoCtx *ctx){
    status r = READY;

    Iter it;
    Iter_Init(&it, tbl);
    while((Iter_Next(&it) & END) == 0){
        Abstract *a = Iter_Get(&it);
        if(a != NULL){
            DoFunc func = Lookup_Get(MemLocalToChain, a->type.of);
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
