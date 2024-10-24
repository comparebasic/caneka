#include <external.h>
#include <caneka.h>

MemCtx *MemKeyed_Make(MemCtx *m){
    MemCtx *md = MemCtx_Make();
    md->instance = Span_Make(m, TYPE_TABLE);
    md->type.state |= TRACKED;
    return m;
}

status MemKeyed_SetKey(MemCtx *m, Abstract *key){
    return Table_SetKey((Span *)m->instance, key);
}
status MemKeyed_Alloc(MemCtx *m, Abstract *key){
    return Table_SetValue((Span *)m->instance, key);
}

