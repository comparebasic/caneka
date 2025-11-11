#include <external.h>
#include <caneka.h>

void *NodeObj_Att(NodeObj *nobj, void *key){
    Object *obj = (Object *)Object_GetPropByIdx(nobj, NODEOBJ_PROPIDX_ATTS);
    if(obj != NULL){
        return Object_Get(obj, key);
    }
    return NULL;
}

status NodeObj_ClsInit(MemCh *m){
    status r = READY;
    ClassDef *cls = ClassDef_Make(m);
    cls->type.of = TYPE_NODEOBJ;
    cls->name = Str_CstrRef(m, "NodeObj");
    Class_SetupProp(cls, Str_CstrRef(m, "name")); /* StrVec */
    Class_SetupProp(cls, Str_CstrRef(m, "atts")); /* Object */
    Class_SetupProp(cls, Str_CstrRef(m, "value")); /* StrVec body, or Number */
    r |= Class_Register(m, cls);
    return r;
}
