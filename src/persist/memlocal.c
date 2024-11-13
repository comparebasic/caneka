#include <external.h>
#include <caneka.h>

status MemLocal_GetLocal(MemCtx *m, void *addr, LocalPtr *lptr){
    memset(lptr, 0, sizeof(LocalPtr));
    MemSlab *sl = MemCtx_GetSlab(m, addr);
    if(sl != NULL){
        lptr->slabIdx = sl->idx;
        lptr->offset = ((void *)addr - (void *)MemSlab_GetStart(sl));
        return SUCCESS;
    }else{
        Fatal("Slab not found, addr outside this memory context\n", TYPE_MEMLOCAL);
    }
    return MISS;
}

static boolean canBeLocal(cls inst){
    return (Ifc_Match(inst, TYPE_STRING) || Ifc_Match(inst, TYPE_SPAN) ||
        Ifc_Match(inst, TYPE_TABLE) || Ifc_Match(inst, TYPE_SINGLE) || inst == TYPE_HASHED);
}

Abstract *MemLocal_Set(MemLocal *ml, String *key, Abstract *a){
    if(a == NULL || key == NULL || !canBeLocal(a->type.of) || (a->type.state & LOCAL_PTR) == 0){
        Fatal("Cannot make instance type local", TYPE_MEMCTX);
        return NULL;
    }

    Hashed *h = Hashed_Make(ml->m, (Abstract *)key);
    h->type.state |= LOCAL_PTR;
    Table_Set(ml->tbl, (Abstract *)h, a);

    return a;
}

Abstract *MemLocal_GetPtr(MemCtx *m, LocalPtr *lptr){
    if(lptr == NULL){
        printf("returning NULL I\n");
        return NULL;
    }
    Iter it;
    if(m->index == NULL || (m->type.state & LOCAL_PTR) == 0){
        Fatal("Index is null or not a LOCAL_PTR MemCtx", TYPE_MEMCTX);
        return NULL;
    }
    MemSlab *sl = Span_Get(m->index, lptr->slabIdx);
    Abstract *a = NULL;
    if(sl != NULL){
       a = MemSlab_GetStart(sl)+lptr->offset; 
    }else{
       Fatal("MemLocal Slab not found", TYPE_MEMLOCAL);
    }

    return a;
}

Abstract *MemLocal_Trans(MemCtx *m, Abstract *a){
    if((m->type.state & LOCAL_PTR) != 0){
        return MemLocal_GetPtr(m, (LocalPtr *)&a);
    }

    return a;
}

MemLocal *MemLocal_Load(MemCtx *m, String *index, IoCtx *ctx){
    MemLocal *ml = MemLocal_MakeBare(m);

    int idx = 0;
    File file;
    String *path = IoCtx_GetPath(m, ctx, index);
    String_AddBytes(m, path, bytes("."), 1);
    i64 l = path->length;
    int max = 10;
    while(1){
        path->length = l;
        String_Add(m, path, String_FromInt(m, idx)); 
        File_Init(&file, path, ctx->access, NULL);
        File_Load(m, &file, ctx->access);
        if((file.type.state & NOOP) == 0){
            MemSlab *sl = MemSlab_Make(NULL);
            String_ToSlab(file.data, (void *)sl,  sizeof(MemSlab)); 
            MemSlab_Attach(ml->m, sl);
        }else{
            break;
        }
        idx++;
        if(idx > max){
            break;
        }
    }

    MemLocal_Awake(ml);
    if((ml->type.state & ERROR) == 0){
        return ml;
    }

    return NULL;
}

status MemLocal_Persist(MemCtx *m, MemLocal *mstore, IoCtx *ctx){
    Iter it;
    Iter_Init(&it, mstore->m->index);
    String *s = IoCtx_GetMstorePath(m, ctx);
    File f;
    while(Iter_Next(&it) != END){
        MemSlab *sl = (MemSlab *)Iter_Get(&it);
        File_Init(&f, s, ctx->access, NULL);
        if(sl != NULL){
            String *fname = String_Init(m, STRING_EXTEND);
            String_AddBytes(m, fname, bytes("memslab."), strlen("memslab."));
            String_Add(m, fname, String_FromInt(m, sl->idx));

            s = IoCtx_GetPath(m, ctx, fname);
            File_Init(&f, s, ctx->access, NULL);
            f.data = String_Init(m, sizeof(MemSlab));

            String_AddBytes(m, f.data, (byte *)sl, sizeof(MemSlab));
            f.type.state |= FILE_UPDATED;

            File_Persist(m, &f);
        }
    }
    
    return SUCCESS;
}


status MemLocal_Awake(MemLocal *ml){
    LocalPtr lp;
    lp.slabIdx = 0;
    lp.offset = 0;
    ml->tbl = (Span *)MemLocal_GetPtr(ml->m, &lp);
    if(ml->tbl != NULL && ml->tbl->type.of == TYPE_TABLE){
        ml->tbl->m = ml->m;
        return SUCCESS;
    }else{
        ml->type.state |= ERROR;
        ml->type.state &= ~SUCCESS;
        return ml->type.state;
    }
}

MemLocal *MemLocal_MakeBare(MemCtx *_m){
    MemCtx *m = (MemCtx *)MemCtx_Make();
    m->type.state |= LOCAL_PTR;
    m->index = Span_Make(_m, TYPE_MEM_SPAN);

    MemLocal *ml = (MemLocal*)MemCtx_Alloc(_m, sizeof(MemLocal));
    ml->type.of = TYPE_MEMLOCAL;
    ml->m = m;
    return ml;
}

MemLocal *MemLocal_Make(MemCtx *_m){
    MemLocal *ml = MemLocal_MakeBare(_m);
    ml->tbl = Span_Make(ml->m, TYPE_TABLE);

    return ml;
}
