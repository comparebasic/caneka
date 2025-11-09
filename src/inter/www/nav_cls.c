#include <external.h>
#include <caneka.h>

status Nav_Add(Nav *nav, StrVec *path, void *a){
    Object *obj = (Object *)Object_As(nav, TYPE_HTML_NAV);
    obj->type.state |= DEBUG;
    return Object_ByPath(obj, path, a, SPAN_OP_SET) != NULL ? SUCCESS : ERROR;
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
