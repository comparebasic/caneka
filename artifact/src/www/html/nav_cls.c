#include <external.h>
#include <caneka.h>
#include <www.h>

static i64 Nav_Print(Stream *sm, Abstract *a, cls type, word flags){
    OrdTable *ot = (OrdTable *)as(a, TYPE_ORDTABLE);
    Abstract *args[] = {
        (Abstract *)I32_Wrapped(sm->m, ot->order->nvalues-1),
        NULL
    };
    i8 indent = 1;
    i64 total = Fmt(sm, "Www.Nav<^D.$^d.count\n", args);
    Iter it;
    Iter_Init(&it, ot->order);
    Single *sg = I32_Wrapped(sm->m, 0);
    boolean nested = FALSE;
    sm->indent++;
    while((Iter_Next(&it) & END) == 0){
        sg->val.i = it.idx;
        Abstract *args[] = {
            (Abstract *)sg,
            (Abstract *)Iter_Get(&it),
            NULL
        };
        total += Stream_IndentOut(sm);
        if(flags & DEBUG){
            total += Fmt(sm, "$: &\n", args);
        }else{
            total += Fmt(sm, "$: @\n", args);
        }
    }
    sm->indent--;
    total += Stream_IndentOut(sm);
    total += Stream_Bytes(sm, (byte *)">", 1);
    return total;
}

status Nav_Add(Nav *nav, StrVec *path, Abstract *a){
    OrdTable *ot = (OrdTable *)as(nav->val.ptr, TYPE_ORDTABLE); 
    return OrdTable_AddByPath(ot, path, a);
}

Nav *Nav_Make(MemCh *m){
    return Ptr_Wrapped(m, OrdTable_Make(m), TYPE_HTML_NAV);
}

Abstract *Nav_GetIndex(MemCh *m, FetchTarget *tg, Abstract *item, Abstract *source){
    Single *sg = (Single *)as(item, TYPE_WRAPPED_PTR);
    OrdTable *ot = (OrdTable *)as(sg->val.ptr, TYPE_ORDTABLE); 
    return Span_Get(ot->order, 0);
}

Abstract *Nav_PathsIter(MemCh *m, FetchTarget *tg, Abstract *item, Abstract *source){
    Single *sg = (Single *)as(item, TYPE_WRAPPED_PTR);
    OrdTable *ot = (OrdTable *)as(sg->val.ptr, TYPE_ORDTABLE); 
    Iter *it = Iter_Make(m, ot->order);
    Iter_Next(it);
    return (Abstract *)it;
}

status Nav_ClsInit(MemCh *m){
    status r = READY;
    ClassDef *cls = ClassDef_Make(m);
    cls->objType.of = TYPE_HTML_NAV;
    cls->parentType.of = TYPE_ORDTABLE;
    cls->name = Str_CstrRef(m, "Nav");
    cls->getIter = Nav_PathsIter;
    cls->methods = Table_Make(m);
    cls->toS = Nav_Print;
    Table_Set(cls->methods, (Abstract *)Str_CstrRef(m, "index"), (void *)Nav_GetIndex);

    r |= Class_Register(m, cls);
    return r;
}
