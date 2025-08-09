#include <external.h>
#include <caneka.h>
#include <www.h>

Nav *Nav_Make(MemCh *m){
    return Ptr_Wrapped(m, Span_Make(m), TYPE_HTML_NAV);
}

Abstract *Nav_GetIndex(MemCh *m, FetchTarget *tg, Abstract *item, Abstract *source){
    Span *p = (Span *)as(item, TYPE_SPAN); 
    return Span_Get(p, 0);
}

Abstract *Nav_PathsIter(MemCh *m, FetchTarget *tg, Abstract *item, Abstract *source){
    Span *p = (Span *)as(item, TYPE_SPAN); 
    Iter *it = Iter_Make(m, p);
    Iter_Next(it);
    return (Abstract *)it;
}

status Nav_ClsInit(MemCh *m){
    ClassDef *cls = ClassDef_Make(m);
    cls->objType.of = TYPE_HTML_NAV;
    cls->parentType.of = TYPE_SPAN;
    cls->name = Str_CstrRef(m, "Nav");
    cls->getIter = Nav_PathsIter;
    cls->methods = Table_Make(m);
    Table_Set(cls->methods, (Abstract *)Str_CstrRef(m, "index"), (void *)Nav_GetIndex);

    Class_Register(m, cls);
    return NOOP;
}
