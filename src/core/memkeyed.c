#include <external.h>
#include <caneka.h>

MemCtx *MemKeyed_Make(MemCtx *m){
    MemCtx *md = MemCtx_Make();
    md->index = Span_Make(m, TYPE_MEM_SPAN);
    md->type.state |= TRACKED;
    return md;
}

status MemKeyed_Set(MemCtx *m, Span *tbl, Abstract *key, Abstract *value){
    Table_SetKey(tbl, key);
    Coords coords;
    memcpy(&coords, &m->latest, sizeof(Coords));
    Hashed *h = Table_SetValue(tbl, (Abstract *)coords.ptr);
    if(h != NULL){
        h->locationIdx = coords.slabIdx;
        h->offset = coords.offset;
    }

    return SUCCESS;
}

status MemKeyed_Persist(MemCtx *m, IoCtx *ctx){
    return SUCCESS;
}

