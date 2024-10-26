#include <external.h>
#include <caneka.h>

MemCtx *MemKeyed_Make(MemCtx *m, Span *tbl){
    MemCtx *md = MemCtx_Make();
    md->instance = tbl;
    md->type.state |= TRACKED;
    return md;
}

status MemKeyed_SetKey(MemCtx *m, Abstract *key){
    return Table_SetKey((Span *)m->instance, key);
}
status MemKeyed_Alloc(MemCtx *m, Abstract *key){
    return Table_SetValue((Span *)m->instance, key);
}
