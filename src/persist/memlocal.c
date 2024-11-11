#include <external.h>
#include <caneka.h>

Chain *MemLocalChain = NULL;

static Abstract *StringLocalPtr(MemCtx *m, Abstract *a){
    /* TODO: reformat strings to be absolute */
    a->type.state &= ~LOCAL_PTR;
    return a; 
}

static status populateMemLocal(MemCtx *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_STRING, (void *)StringLocalPtr);
    
    return r;
}

status MemLocal_Init(MemCtx *m){
    if(MemLocalChain == NULL){
        Lookup *funcs = Lookup_Make(m, _TYPE_START, populateMemLocal, NULL);
        MemLocalChain = Chain_Make(m, funcs);
        return SUCCESS;
    }
    return NOOP;

}

LocalPtr *MemLocal_GetLocal(MemCtx *m, void *addr){
    MemSlab *sl = MemCtx_GetSlab(m, addr);
    if(sl != NULL){
        return (LocalPtr *)((void *)addr - (void *)MemSlab_GetStart(sl));
    }
    return NULL;
}

Abstract *MemLocal_GetPtr(MemCtx *m, LocalPtr *lptr){
    Iter it;
    MemSlab *sl = Span_Get(m->index, lptr->slabIdx);
    Abstract *a = NULL;
    Maker func = NULL;
    if(sl != NULL){
       a = MemSlab_GetStart(sl)+lptr->offset; 
    }
    if(a != NULL && (a->type.state & LOCAL_PTR) != 0){
        func = (Maker)Chain_Get(DebugPrintChain, a->type.of);
        if(func != NULL){
            func((MemHandle *)m, a);
        }
    }

    return a;
}

MemCtx *MemLocal_FromIndex(MemCtx *m, String *index, IoCtx *ctx){
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

status MemLocal_Persist(MemCtx *m, MemCtx *mstore, IoCtx *ctx){
    Iter it;
    Debug_Print((void *)mstore->index, 0, "Index of slabs:", COLOR_CYAN, TRUE);
    printf("\n");
    Iter_Init(&it, mstore->index);
    String *s = IoCtx_GetMstorePath(m, ctx);
    while(Iter_Next(&it) != END){
        MemSlab *sl = (MemSlab *)Iter_Get(&it);
        File *f = File_Make(m, s, ctx->access, NULL);
        if(sl != NULL){
            String *fname = String_Init(m, STRING_EXTEND);
            String_AddBytes(m, fname, bytes("memslab."), strlen("memslab."));
            String_Add(m, fname, String_FromInt(m, sl->idx));

            s = IoCtx_GetPath(m, ctx, fname);
            File *f = File_Make(m, s, ctx->access, NULL);
            f->data = String_Init(m, sizeof(MemSlab));

            String_AddBytes(m, f->data, (byte *)sl, sizeof(MemSlab));
            f->type.state |= FILE_UPDATED;

            File_Persist(m, f);
        }
    }
    
    return SUCCESS;
}

MemCtx *MemLocal_Make(){
    MemCtx *m = (MemCtx *)MemCtx_Make();
    m->type.state |= LOCAL_PTR;
    m->index = Span_Make(m, TYPE_MEM_SPAN);

    return m;
}
