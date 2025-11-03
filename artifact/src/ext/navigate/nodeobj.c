#include <external.h>
#include <caneka.h>

status NodeObj_ClsInit(MemCh *m){
    status r = READY;
    ClassDef *cls = ClassDef_Make(m);
    cls->objType.of = TYPE_NODEOBJ;
    cls->name = Str_CstrRef(m, "NodeObj");
    Class_SetupProp(cls, Str_CstrRef(m, "name"));
    Class_SetupProp(cls, Str_CstrRef(m, "atts"));
    Class_SetupProp(cls, Str_CstrRef(m, "value"));
    r |= Class_Register(m, cls);
    return r;
}
