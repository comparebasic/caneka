#include <external.h>
#include <caneka.h>

void *NodeObj_Att(Inst *nobj, void *key){
    if(nobj == NULL){
        return NULL;
    }
    Table *obj = (Table *)Span_Get(nobj, NODEOBJ_PROPIDX_ATTS);
    if(obj != NULL){
        return Table_Get(obj, key);
    }
    return NULL;
}

Table *NodeObj_GetTblOfAtt(Inst *nobj, void *key){
    if(nobj == NULL){
        return NULL;
    }

    Table *tbl = NodeObj_Att(nobj, key);
    if(tbl == NULL){
        tbl = Table_Make(nobj->m);
        NodeObj_SetAtt(nobj, key, tbl);
    }
    return tbl;
}

status NodeObj_SetAtt(Inst *nobj, void *key, void *value){
    if(nobj == NULL){
        return ERROR;
    }
    Table *obj = (Table *)Span_Get(nobj, NODEOBJ_PROPIDX_ATTS);
    if(obj != NULL){
        return Table_Set(obj, key, value);
    }
    return ERROR;
}

void *NodeObj_GetChild(Inst *nobj, void *key){
    if(nobj == NULL){
        return NULL;
    }
    Table *children = (Table *)Span_Get(nobj, NODEOBJ_PROPIDX_CHILDREN);
    return Table_Get(children, key);
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
