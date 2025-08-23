#include <external.h>
#include <caneka.h>

static i64 Nav_Print(Stream *sm, Abstract *a, cls type, word flags){
    Nav *nav = (Nav *)Object_As((Object *)a, TYPE_OBJECT);
    Abstract *args[] = {
        (Abstract *)I32_Wrapped(sm->m, nav->order->nvalues),
        NULL
    };
    i8 indent = 1;
    i64 total = Fmt(sm, "Www.Nav<^D.$^d.count\n", args);
    Iter it;
    Iter_Init(&it, nav->order);
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
            if(Object_TypeMatch(h->value, TYPE_HTML_NAV)){
                total += Nav_Print(sm, h->value, TYPE_HTML_NAV, flags);
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
    cls->api.toS = Nav_Print;
    Class_SetupProp(cls, Str_CstrRef(m, "index"));
    r |= Class_Register(m, cls);
    return r;
}
