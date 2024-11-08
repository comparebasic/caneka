#include <external.h>
#include <caneka.h>

static status memKeyed_SanatizeSlab(MemKeyed *mk, String *stringSl){
    return SUCCESS;
}

status MemKeyed_Set(MemCtx *m, MemKeyed *mk, Abstract *key, Abstract *value){
    Table_SetKey(mk->tbl, key);
    Coords coords;
    memcpy(&coords, &mk->m->latest, sizeof(Coords));
    Hashed *h = Table_SetValue(mk->tbl, (Abstract *)coords.ptr);
    if(h != NULL){
        h->locationIdx = coords.slabIdx;
        h->offset = coords.offset;
    }

    if(value->type.of == TYPE_STRING_CHAIN){
        /* add referenced segments here */
        printf("Gotta redo a ref");
    }

    return SUCCESS;
}

MemKeyed *MemKeyed_FromIndex(MemCtx *m, String *index, IoCtx *ctx){
    printf("index is: \x1b[%dm%s\n", COLOR_YELLOW, index->bytes);

    File *file = File_Make(m, index, NULL, NULL);
    File_Load(m, file, NULL, ctx);

    printf("index content: \x1b[%dm'%s'\x1b[0m\n", COLOR_YELLOW, file->data->bytes);

    return NULL;
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
            String_AddBytes(m, index, bytes("/"), 1);
            String_Add(m, index, String_FromInt(m, h->offset));
            String_AddBytes(m, index, bytes("\n"), 1);
        }
    }
    
    String *s = IoCtx_GetMstorePath(m, ctx);
    File *f = File_Make(m, s, ctx->access, NULL);
    f->data = index;
    f->type.state |= FILE_UPDATED;
    File_Persist(m, f);

    Debug_Print((void *)mstore->m->index, 0, "Index of slabs:", COLOR_CYAN, TRUE);
    printf("\n");
    Iter_Init(&it, mstore->m->index);
    while(Iter_Next(&it) != END){
        MemSlab *sl = (MemSlab *)Iter_Get(&it);
        f = File_Make(m, s, ctx->access, NULL);
        if(sl != NULL){
            String *fname = String_Init(m, STRING_EXTEND);
            String_AddBytes(m, fname, bytes("memslab."), strlen("memslab."));
            String_Add(m, fname, String_FromInt(m, sl->idx));

            s = IoCtx_GetPath(m, ctx, fname);
            File *f = File_Make(m, s, ctx->access, NULL);
            f->data = String_Init(m, sizeof(MemSlab));

            String_AddBytes(m, f->data, (byte *)sl, sizeof(MemSlab));
            memKeyed_SanatizeSlab(mstore, f->data);
            f->type.state |= FILE_UPDATED;

            File_Persist(m, f);
        }
    }
    
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
