#include <external.h>
#include <caneka.h>

Abstract *NodeObj_Att(NodeObj *nobj, Abstract *key){
    Table *tbl = (Table *)Object_GetPropByIdx(nobj, NODEOBJ_PROPIDX_ATTS);
    if(tbl != NULL){
        return Table_Get(tbl, key);
    }
    return NULL;
}

status NodeObj_ClsInit(MemCh *m){
    status r = READY;
    ClassDef *cls = ClassDef_Make(m);
    cls->objType.of = TYPE_NODEOBJ;
    cls->name = Str_CstrRef(m, "NodeObj");
    Class_SetupProp(cls, Str_CstrRef(m, "name")); /* StrVec */
    Class_SetupProp(cls, Str_CstrRef(m, "atts")); /* Table */
    Class_SetupProp(cls, Str_CstrRef(m, "value")); /* StrVec body, or Number */
    r |= Class_Register(m, cls);
    return r;
}
