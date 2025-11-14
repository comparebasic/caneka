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

    Table_Set(tbl, S(m, "name"), I16_Wrapped(m, TYPE_STRVEC));
    Table_Set(tbl, S(m, "atts"), I16_Wrapped(m, TYPE_TABLE));
    Hashed *h = Table_SetHashed(tbl, S(m, "children"), I16_Wrapped(m, TYPE_TABLE));

    r |= Seel_Seel(m, tbl, S(m, "NodeObj"), TYPE_NODEOBJ, h->orderIdx);

    return r;
}
