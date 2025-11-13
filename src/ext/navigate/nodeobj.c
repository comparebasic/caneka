#include <external.h>
#include <caneka.h>

void *NodeObj_Att(Inst *nobj, void *key){
    Table *obj = (Table *)Span_Get(nobj, NODEOBJ_PROPIDX_ATTS);
    if(obj != NULL){
        return Table_Get(obj, key);
    }
    return NULL;
}

status NodeObj_ClsInit(MemCh *m){
    status r = READY;
    Table *tbl = Table_Make(m);

    Table_Set(tbl, Str_FromCstr(m, "name"), STRING_COPY, I16_Wrapped(m, TYPE_STRVEC));
    Table_Set(tbl, Str_FromCstr(m, "atts"), STRING_COPY, I16_Wrapped(m, TYPE_INSTANCE));
    Table_Set(tbl, Str_FromCstr(m, "value"), STRING_COPY, I16_Wrapped(m, TYPE_STRVEC));
    Table_Set(tbl, Str_FromCstr(m, "children"), STRING_COPY, I16_Wrapped(m, TYPE_TABLE));

    r |= Seal_Seal(m, tbl, Str_FromCstr(m, "NodeObj", STRING_COPY ), TYPE_NODEOBJ);

    return r;
}
