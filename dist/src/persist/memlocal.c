#include <external.h>
#include <caneka.h>

Chain *MemLocalToChain = NULL;
Chain *MemLocalFromChain = NULL;
Lookup *ExemptLocal = NULL;

static status MemLocal_addTo(MemCtx *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_STRING, (void *)String_ToLocal);
    r |= Lookup_Add(m, lk, TYPE_SPAN, (void *)Span_ToLocal);
    r |= Lookup_Add(m, lk, TYPE_WRAPPED_PTR, (void *)WrappedPtr_ToLocal);
    r |= Lookup_Add(m, lk, TYPE_HASHED, (void *)Hashed_ToLocal);
    return r;
}

static status MemLocal_addFrom(MemCtx *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_STRING_CHAIN+HTYPE_LOCAL, (void *)String_FromLocal);
    r |= Lookup_Add(m, lk, TYPE_SPAN+HTYPE_LOCAL, (void *)Span_FromLocal);
    r |= Lookup_Add(m, lk, TYPE_WRAPPED_PTR+HTYPE_LOCAL, (void *)WrappedPtr_FromLocal);
    r |= Lookup_Add(m, lk, TYPE_HASHED+HTYPE_LOCAL, (void *)Hashed_FromLocal);
    return r;
}

static status MemLocal_addExempt(MemCtx *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_WRAPPED, (void *)TRUE);
    r |= Lookup_Add(m, lk, TYPE_STRING_FIXED, (void *)TRUE);
    r |= Lookup_Add(m, lk, TYPE_STRING_FULL, (void *)TRUE);
    return r;
}

status MemLocal_Init(MemCtx *m){
    if(MemLocalToChain == NULL){
        Lookup *funcs = Lookup_Make(m, _TYPE_START, MemLocal_addTo, NULL);
        MemLocalToChain = Chain_Make(m, funcs);

        funcs = Lookup_Make(m, _TYPE_START, MemLocal_addFrom, NULL);
        MemLocalFromChain = Chain_Make(m, funcs);

        ExemptLocal = Lookup_Make(m, _TYPE_START, MemLocal_addExempt, NULL);
        return SUCCESS;
    }

    return NOOP;
}

status MemLocal_To(MemCtx *m, Abstract *a){
    if(a == NULL || a->type.of > HTYPE_LOCAL){
        return NOOP;
    }
    DoFunc func = Chain_Get(MemLocalToChain, Ifc_Get(a->type.of));
    if(func == NULL){
        if(Lookup_Get(ExemptLocal, Ifc_Get(a->type.of)) != NULL){
            return NOOP;
        }
        Fatal("Unable to find To conversion to MemLocal Abstract", TYPE_MEMLOCAL);
        return ERROR;
    }
    return func(m, a); 
}

status MemLocal_From(MemCtx *m, Abstract *a){
    if(a == NULL || a->type.of < HTYPE_LOCAL){
        return NOOP;
    }
    DoFunc func = Chain_Get(MemLocalFromChain, Ifc_Get(a->type.of));
    if(func == NULL){
        if(Lookup_Get(ExemptLocal, Ifc_Get(a->type.of)) != NULL){
            return NOOP;
        }
        Fatal("Unable to find From conversion to MemLocal Abstract", TYPE_MEMLOCAL);
        return ERROR;
    }
    return func(m, a); 
}

status MemLocal_SetLocal(MemCtx *m, Abstract **addr){
    if(*addr == NULL){
        return NOOP;
    }
    boolean subTo = TRUE;
    if((*addr)->type.of == TYPE_MEMLOCAL_SETTER){
        addr = (Abstract **)(*((LocalSetter **)addr))->dptr;
        subTo = FALSE;
    }
    DebugStack_Push("MemLocal_SetLocal", TYPE_CSTR);
    MemSlab *sl = MemCtx_GetSlab(m, addr);
    LocalPtr *lptr = (LocalPtr *)addr;
    memset(lptr, 0, sizeof(LocalPtr));
    if(sl != NULL){
        if(subTo){
            MemLocal_To(m, (Abstract *)*addr);
        }
        lptr->slabIdx = sl->idx;
        lptr->offset = ((void *)addr - (void *)sl->bytes);
        DebugStack_Pop();
        return SUCCESS;
    }else{
        Fatal("Slab not found, addr outside this memory context\n", TYPE_MEMLOCAL);
        DebugStack_Pop();
        return ERROR;
    }

    DebugStack_Pop();
    return NOOP;
}

status MemLocal_UnSetLocal(MemCtx *m, LocalPtr *lptr){
    if(lptr->slabIdx == 0 && lptr->offset == 0){
        return NOOP;
    }
    DebugStack_Push("MemLocal_UnSetLocal", TYPE_CSTR);
    MemSlab *sl = m->start_sl;
    void *ptr = NULL;
    while(sl != NULL){
        if(sl->idx == lptr->slabIdx){
            ptr = (sl->bytes+lptr->offset);
        }
        sl = sl->next;
    }
    if(ptr == NULL){
        DebugStack_Pop();
        return ERROR;
    }
    memcpy(lptr, &ptr, sizeof(void *));
    DebugStack_Pop();
    return SUCCESS;
}

Span *MemLocal_Load(MemCtx *m, String *path, Access *access){
    DebugStack_Push("MemLocal_Load", TYPE_CSTR);
    status r = READY;
    Iter it;

    MemCtx *mlm = MemCtx_Make();
    String_AddBytes(m, path, bytes("/memslab."), strlen("/memslab."));
    i64 l = path->length;

    File slabFile;
    int idx = 0;
    String_Add(m, path, String_FromInt(m, idx));

    while(File_Exists(path) & SUCCESS){
        File_Init(&slabFile, path, NULL, NULL);
        slabFile.abs = slabFile.path;
        slabFile.data = String_Init(m, sizeof(MemSlab));
        File_Stream(m, &slabFile, access, NULL, NULL); 

        i64 offset = 0;
        MemSlab *sl = MemSlab_Make(m, 0);
        String *s = slabFile.data;
        while(s != NULL){
            memcpy(((void *)sl)+offset, s->bytes, s->length);
            offset += s->length;
            s = String_Next(s);
        }
        MemSlab_Attach(mlm, sl);

        String_Trunc(slabFile.path, l);
        String_Add(m, path, String_FromInt(m, ++idx));
    }

    Span *ml = as(mlm->start_sl->bytes, TYPE_SPAN);

    Iter_Init(&it, ml);
    while((Iter_Next(&it) & END) == 0){
        Abstract *item = asIfc(Iter_Get(&it), TYPE_MEMLOCAL_ITEM);
        if(item != NULL){
            r |= MemLocal_To(ml->m, item);
            if((r & ERROR) != 0){
                break;
            }
        }
    }

    DebugStack_Pop();
    return ml;
}

status MemLocal_Destroy(MemCtx *m, String *path, Access *access){
    return Dir_Destroy(m, path, access);
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

    String *fname = String_Clone(m, path);
    String_AddBytes(m, fname, bytes("/memslab."), strlen("/memslab."));
    i64 l = fname->length;

    File f;
    MemSlab *sl = ml->m->start_sl;
    while(sl != NULL){
        File_Init(&f, fname, access, NULL);
        fname->length = l;
        String_Add(m, fname, String_FromInt(m, sl->idx));

        File_Init(&f, String_Clone(m, fname), access, NULL);
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

Span *MemLocal_Make(cls typeOf){
    DebugStack_Push("MemLocal_Make", TYPE_CSTR);
    if(Ifc_Match(typeOf, TYPE_SPAN)){
        MemCtx *m = MemCtx_Make();
        DebugStack_Pop();
        return Span_Make(m, typeOf);
    }else{
        Fatal("MemLocal typeOf not found as possible root types", TYPE_MEMLOCAL);
        DebugStack_Pop();
        return NULL;
    }
}
