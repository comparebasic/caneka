#include <external.h>
#include <caneka.h>

LocalPtr *MemLocal_GetLocal(MemCtx *m, void *addr){
    MemSlab *sl = MemCtx_GetSlab(m, addr);
    if(sl != NULL){
        return (LocalPtr *)((void *)addr - (void *)MemSlab_GetStart(sl));
    }
    return NULL;
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

    Debug_Print((void *)key, 0,  "Making hash key: \n", COLOR_RED, TRUE);
    printf("\n");
    Hashed *h = Hashed_Make(ml->m, (Abstract *)key);
    h->type.state |= LOCAL_PTR;
    Debug_Print((void *)h, 0,  "Inserting Hashed: \n", COLOR_RED, TRUE);
    Table_Set(ml->tbl, (Abstract *)h, (Abstract *)MemLocal_GetLocal(ml->m, a));

    return a;
}

Abstract *MemLocal_GetPtr(MemCtx *m, LocalPtr *lptr){
    if(lptr == NULL){
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
    }

    return a;
}

Abstract *MemLocal_Trans(MemCtx *m, Abstract *a){
    if((m->type.state & LOCAL_PTR) != 0){
        return MemLocal_GetPtr(m, (LocalPtr *)a);
    }

    return a;
}

MemLocal *MemLocal_FromIndex(MemCtx *m, String *index, IoCtx *ctx){
    printf("index is: \x1b[%dm%s\n", COLOR_YELLOW, index->bytes);
    int idx = 0;
    File *file = NULL;
    String *path = String_Init(m, STRING_EXTEND);
    String_AddBytes(m, path, bytes("."), 1);
    i64 l = path->length;
    while(1){
        path->length = l;
        String_Add(m, path, String_FromInt(m, idx)); 
        file = File_Make(m, path, ctx->access, NULL);
        if((file->type.state & NOOP) == 0){
            printf("Found memslab %d\n", idx);
        }else{
            break;
        }
        idx++;
    }

    return NULL;
}

status MemLocal_Persist(MemCtx *m, MemLocal *mstore, IoCtx *ctx){
    Iter it;
    Debug_Print((void *)mstore->m->index, 0, "Index of slabs:", COLOR_CYAN, TRUE);
    printf("\n");
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

MemLocal *MemLocal_Make(){
    MemCtx *m = (MemCtx *)MemCtx_Make();
    m->type.state |= LOCAL_PTR;
    m->index = Span_Make(m, TYPE_MEM_SPAN);

    MemLocal *ml = (MemLocal*)MemCtx_Alloc(m, sizeof(MemLocal));
    ml->m = m;
    ml->tbl = Span_Make(m, TYPE_TABLE);

    return ml;
}
