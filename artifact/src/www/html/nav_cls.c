#include <external.h>
#include <caneka.h>

static i64 Nav_Print(Stream *sm, Abstract *a, cls type, word flags){
    PathTable *pt = (PathTable *)as(a, TYPE_PATHTABLE);
    Abstract *args[] = {
        (Abstract *)I32_Wrapped(sm->m, pt->order->nvalues),
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
        Abstract *item = Iter_Get(&it);
        total += Stream_IndentOut(sm);
        if(item->type.of == TYPE_HASHED){
            Hashed *h = (Hashed *)item;
            Abstract *args[] = {
                (Abstract *)sg,
                (Abstract *)h->key,
                NULL
            };
            total += Fmt(sm, "$: @ -> ", args);
            if(h->value->type.of == TYPE_PATHTABLE){
                total += Nav_Print(sm, h->value, h->value->type.of, flags);
                Stream_Bytes(sm, (byte *)"\n", 1);
            }else if(h->value->type.of == TYPE_WRAPPED_PTR &&
                    ((Single *)h->value)->objType.of == TYPE_HTML_NAV){
                Single *inst = (Single *)h->value;
                total += Nav_Print(sm, inst->val.ptr, TYPE_HTML_NAV, flags);
                Stream_Bytes(sm, (byte *)"\n", 1);
            }else if(flags & DEBUG){
                Abstract *args[] = {
                    (Abstract *)sg,
                    (Abstract *)h->value,
                    NULL
                };
                total += Fmt(sm, "$: &\n", args);
            }else{
                Abstract *args[] = {
                    (Abstract *)sg,
                    (Abstract *)h->value,
                    NULL
                };
                total += Fmt(sm, "$: @\n", args);
            }
        }
    }
    sm->indent--;
    total += Stream_IndentOut(sm);
    total += Stream_Bytes(sm, (byte *)">", 1);
    return total;
}

status Nav_Add(Nav *nav, StrVec *path, Abstract *a){
    PathTable *pt = (PathTable *)as(nav->val.ptr, TYPE_PATHTABLE); 
    return PathTable_AddByPath(pt, path, a);
}

Nav *Nav_Make(MemCh *m){
    PathTable *pt = PathTable_Make(m);
    pt->cls = (ClassDef *)Lookup_Get(ClassLookup, TYPE_HTML_NAV);
    return Ptr_Wrapped(m, (Abstract *)pt, TYPE_HTML_NAV);
}

Abstract *Nav_PathsIter(MemCh *m, FetchTarget *tg, Abstract *item, Abstract *source){
    Object *o = (Single *)asObj(item, TYPE_HTML_NAV);
    return Class_GetIter(o, pt->cls);
}

status Nav_ClsInit(MemCh *m){
    status r = READY;
    ClassDef *cls = ClassDef_Make(m);
    cls->type.inst = TYPE_HTML_NAV;
    cls->name = Str_CstrRef(m, "Nav");
    cls->getIter = Nav_PathsIter;
    cls->toS = Nav_Print;
    Class_SetupProp(cls, Str_CstrRef(m, "index"));
    r |= Class_Register(m, cls);
    return r;
}
