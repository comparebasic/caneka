#include <external.h>
#include <caneka.h>

Abstract *NavItem_Make(MemCh *m, TranspFile *tp){
    Hashed *h = Hashed_Make(m, (Abstract *)tp->name);
    h->value = Ptr_Wrapped(m, tp, TYPE_HTTP_NAVITEM);
    return h;
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
    cls->objType.of = TYPE_HTTP_NAVITEM;
    cls->parentType.of = TYPE_SPAN;
    cls->name = Str_CstrRef(m, "Nav");
    cls->getIter = Nav_PathsIter;
    cls->methods = Table_Make(m);
    Table_Set(cls->methods, Str_CstrRef(m, "index"), (void *)Nav_GetIndex);

    Class_Register(m, cls);
    return NOOP;
}
