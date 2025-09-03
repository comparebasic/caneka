
#include <external.h>
#include <caneka.h>

static i64 Nav_Print(Stream *sm, Abstract *a, cls type, word flags){
    Page *pg = (Page *)Object_As((Object *)a, TYPE_OBJECT);
    i64 total = 0;
    return total;
}

status Nav_ClsInit(MemCh *m){
    status r = READY;
    ClassDef *cls = ClassDef_Make(m);
    cls->objType.of = TYPE_HTML_PAGE;
    cls->name = Str_CstrRef(m, "HtmlPage");
    cls->api.toS = Page_Print;
    Class_SetupProp(cls, Str_CstrRef(m, "url"));
    Class_SetupProp(cls, Str_CstrRef(m, "name"));
    Class_SetupProp(cls, Str_CstrRef(m, "body"));
    r |= Class_Register(m, cls);
    return r;
}
