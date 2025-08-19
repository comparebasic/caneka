#include <external.h>
#include <caneka.h>

static i64 Nav_Print(Stream *sm, Abstract *a, cls type, word flags){
    PathTable *pt = (PathTable *)as(a, TYPE_PATHTABLE);
    Abstract *args[] = {
        (Abstract *)I32_Wrapped(sm->m, pt->order->nvalues-1),
        NULL
    };
    i8 indent = 1;
    i64 total = Fmt(sm, "Www.Nav<^D.$^d.count\n", args);
    Iter it;
    Iter_Init(&it, pt->order);
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

status Nav_SetStatus(MemCh *m, Abstract *a, status flags){
    status r = READY;
    Single *sg = (Single *)as(a, TYPE_WRAPPED_PTR);
    PathTable *pt = (PathTable *)as(sg->val.ptr, TYPE_PATHTABLE); 
    if(flags & DEBUG){
        sg->objType.state |= DEBUG;
        pt->type.state |= DEBUG;
        r |= SUCCESS;
    }
    if(r == READY){
        r |= NOOP;
    }
    return r;
}

status Nav_UnSetStatus(MemCh *m, Abstract *a, status flags){
    status r = READY;
    Single *sg = (Single *)as(a, TYPE_WRAPPED_PTR);
    PathTable *pt = (PathTable *)as(sg->val.ptr, TYPE_PATHTABLE); 
    if((flags & DEBUG) == 0){
        sg->objType.state &= ~DEBUG;
        pt->type.state &= ~DEBUG;
        r |= SUCCESS;
    }
    if(r == READY){
        r |= NOOP;
    }
    return r;
}

status Nav_Add(Nav *nav, StrVec *path, Abstract *a){
    PathTable *pt = (PathTable *)as(nav->val.ptr, TYPE_PATHTABLE); 
    return PathTable_AddByPath(pt, path, a);
}

Nav *Nav_Make(MemCh *m){
    return Ptr_Wrapped(m, PathTable_Make(m), TYPE_HTML_NAV);
}

Abstract *Nav_GetIndex(MemCh *m, FetchTarget *tg, Abstract *item, Abstract *source){
    Single *sg = (Single *)as(item, TYPE_WRAPPED_PTR);
    PathTable *pt = (PathTable *)as(sg->val.ptr, TYPE_PATHTABLE); 
    return Span_Get(pt->order, 0);
}

Abstract *Nav_PathsIter(MemCh *m, FetchTarget *tg, Abstract *item, Abstract *source){
    Single *sg = (Single *)as(item, TYPE_WRAPPED_PTR);
    PathTable *pt = (PathTable *)as(sg->val.ptr, TYPE_PATHTABLE); 
    Iter *it = Iter_Make(m, pt->order);
    Iter_Next(it);
    return (Abstract *)it;
}

status Nav_ClsInit(MemCh *m){
    status r = READY;
    ClassDef *cls = ClassDef_Make(m);
    cls->objType.of = TYPE_HTML_NAV;
    cls->parentType.of = TYPE_PATHTABLE;
    cls->name = Str_CstrRef(m, "Nav");
    cls->getIter = Nav_PathsIter;
    cls->methods = Table_Make(m);
    cls->toS = Nav_Print;
    cls->setStatus = Nav_SetStatus;
    cls->unSetStatus = Nav_UnSetStatus;
    Table_Set(cls->methods, (Abstract *)Str_CstrRef(m, "index"), (void *)Nav_GetIndex);

    r |= Class_Register(m, cls);
    return r;
}
