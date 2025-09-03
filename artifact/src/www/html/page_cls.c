#include <external.h>
#include <caneka.h>

status Page_ClsInit(MemCh *m){
    status r = READY;
    ClassDef *cls = ClassDef_Make(m);
    cls->objType.of = TYPE_HTML_PAGE;
    cls->name = Str_CstrRef(m, "HtmlPage");
    Class_SetupProp(cls, Str_CstrRef(m, "url"));
    Class_SetupProp(cls, Str_CstrRef(m, "name"));
    Class_SetupProp(cls, Str_CstrRef(m, "navName"));
    Class_SetupProp(cls, Str_CstrRef(m, "body"));
    r |= Class_Register(m, cls);
    return r;
}
