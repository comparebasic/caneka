#include <external.h>
#include <caneka.h>

MemCtx *MemKeyed_Make(MemCtx *m){
    MemCtx *md = MemCtx_Make();
    md->instance = (Abstract *)Span_Make(m, TYPE_TABLE);
    md->index = Span_Make(m, TYPE_MEM_SPAN);
    md->type.state |= TRACKED;
    return md;
}

status MemKeyed_SetKey(MemCtx *m, Abstract *key){
    return Table_SetKey((Span *)m->instance, key);
}

status MemKeyed_Persist(MemCtx *m, IoCtx *ctx){
    return SUCCESS;
}

