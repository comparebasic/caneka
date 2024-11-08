#include <external.h>
#include <caneka.h>

status MemKeyed_Set(MemCtx *m, MemKeyed *mk, Abstract *key, Abstract *value){
    Table_SetKey(mk->tbl, key);
    Coords coords;
    memcpy(&coords, &mk->m->latest, sizeof(Coords));
    Hashed *h = Table_SetValue(mk->tbl, (Abstract *)coords.ptr);
    if(h != NULL){
        h->locationIdx = coords.slabIdx;
        h->offset = coords.offset;
    }

    return SUCCESS;
}

status MemKeyed_Persist(MemCtx *m, MemKeyed *mstore, IoCtx *ctx){
    Iter it;
    Iter_Init(&it, mstore->tbl);
    String *index = String_Init(m, STRING_EXTEND);
    while(Iter_Next(&it) != END){
        Hashed *h = (Hashed *)Iter_Get(&it);
        if(h != NULL){
            String_Add(m, index, (String*)h->item);
            String_AddBytes(m, index, bytes(":"), 1);
            String_Add(m, index, String_FromInt(m, h->locationIdx));
            String_AddBytes(m, index, bytes(":"), 1);
            String_Add(m, index, String_FromInt(m, h->offset));
            String_AddBytes(m, index, bytes("\n"), 1);
        }
    }

    
    String *s = IoCtx_GetMstorePath(m, ctx);
    File *f = File_Make(m, s, ctx->access, NULL);
    f->abs = f->path;
    f->data = index;
    f->type.state |= FILE_UPDATED;
    File_Persist(m, f);
    
    return SUCCESS;
}

MemKeyed *MemKeyed_Make(MemCtx *m){
    MemKeyed *mstore = (MemKeyed *)MemCtx_Alloc(m, sizeof(MemKeyed));
    mstore->type.of = TYPE_MEM_KEYED;
    mstore->m = MemCtx_Make();
    mstore->m->index = Span_Make(m, TYPE_MEM_SPAN);
    mstore->tbl = Span_Make(m, TYPE_TABLE);
    return mstore;
}
