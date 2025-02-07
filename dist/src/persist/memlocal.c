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

Span *MemLocal_Load(MemCtx *m, String *path){
    status r = READY;
    Iter it;

    MemCtx *mlm = MemCtx_Make();
    String_AddBytes(m, path, bytes("/memslab."), strlen("/memslab."));
    i64 l = path->length;

    int i = 0;
    File slabFile;
    String_Add(m, path, String_FromInt(m, sl->idx));
    File_Init(&slabFile, path, NULL, NULL);
    slabFile.abs = slabFile.path;
    slabFile.data = String_Init(m, sizeof(MemSlab));
    while((File_Stream(m, &slabFile, access, NULL, NULL) & NOOP) == 0){
        MemSlab_Attach(mlm, (MemSlab *)String_ToChars(m, slabFile.data));
    }

    Span *ml = as(mlm->start_sl->bytes, TYPE_TABLE);

    Iter_Init(&it, ml);
    while((Iter_Next(&it) & END) == 0){
        MemLocalItem *item = asIfc(Iter_Get(&it), TYPE_MEMLOCAL_ITEM);
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

    return ml;
}

status MemLocal_Destroy(MemCtx *m, String *path, IoCtx *ctx){
    return Dir_Destroy(m, IoCtx_GetPath(m, ctx, path));
}

status MemLocal_Persist(MemCtx *m, Span *ml, String *path, Access *access){
    DebugStack_Push("MemLocal_Persist", TYPE_CSTR);
    status r = READY;

    Iter it;
    Iter_Init(&it, ml);
    while((Iter_Next(&it) & END) == 0){
        Abstract *a = Iter_Get(&it);
        if(a != NULL){
            DoFunc func = Chain_Get(MemLocalToChain, a->type.of);
            if(func == NULL){
                Fatal("Unable to find conversion to MemLocal Abstract", TYPE_MEMLOCAL);
                DebugStack_Pop();
                return ERROR;
            }
            r |= func(m, a); 
            if((r & ERROR) != 0){
                break;
            }
        }
    }

    char *path_cstr = String_ToChars(m, path);
    DIR* dir = opendir(path_cstr);
    if(dir){
        closedir(dir);
    }else if(ENOENT == errno){
        if(mkdir(path_cstr, PERMS) != 0){
            Fatal("Unable to make dir", TYPE_IOCTX);
            DebugStack_Pop();
            return ERROR;
        }
    }

    String_AddBytes(m, path, bytes("/memslab."), strlen("/memslab."));
    i64 l = path->length;

    File f;
    MemSlab *sl = ml->m->start_sl;
    while(sl != NULL){
        File_Init(&f, path, access, NULL);
        path->length = l;
        String_Add(m, path, String_FromInt(m, sl->idx));

        File_Init(&f, String_Clone(m, path), access, NULL);
        f.abs = f.path;
        f.data = String_Init(m, sizeof(MemSlab));

        String_AddBytes(m, f.data, (byte *)sl, sizeof(MemSlab));
        f.type.state |= FILE_UPDATED;

        r |= File_Persist(m, &f);
        sl = sl->next;
    }

    DebugStack_Pop();
    return r;
}

Span *MemLocal_Make(word typeOf){
    if(Ifc_Match(ctypeOf, TYPE_SPAN)){
        MemCtx *m = MemCtx_Make();
        return Span_Make(m, typeOf);
    }else{
        Fatal("MemLocal typeOf not found as possible root types", TYPE_MEMLOCAL);
    }
}
