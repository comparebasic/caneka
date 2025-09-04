#include <external.h>
#include <caneka.h>

status Nav_Add(Nav *nav, StrVec *path, Abstract *a){
    Object *obj = (Object *)Object_As(nav, TYPE_HTML_NAV);
    return Object_AddByPath(obj, path, a);
}

Nav *Nav_Make(MemCh *m){
    return Object_Make(m, TYPE_HTML_NAV);
}

status Nav_ClsInit(MemCh *m){
    status r = READY;
    ClassDef *cls = ClassDef_Make(m);
    cls->objType.of = TYPE_HTML_NAV;
    cls->name = Str_CstrRef(m, "Nav");
    Class_SetupProp(cls, Str_CstrRef(m, "index"));
    r |= Class_Register(m, cls);
    return r;
}
