#include <external.h>
#include <caneka.h>

static boolean _init = FALSE;

void *hashedReturnKey(MemCh *m, struct fetch_target *target, void *data, void *source){
    Hashed *h = data;
    return h->key;
}

status Sequence_ClassInit(MemCh *m, Lookup *lk){
    status r = READY;

    if(!_init){
        _init = TRUE;
        /* hashed */
        Hashed h;
        ClassDef *cls = ClassDef_Make(m);
        cls->type.state |= CLASS_NATIVE;
        cls->api.byKey = hashedReturnKey;

        Single *sg = NULL;
        sg = I16_Wrapped(m, (void *)(&h.idx)-(void *)(&h));
        sg->objType.of = TYPE_I32;
        Table_Set(cls->atts,
            Str_CstrRef(m, "idx"), sg);

        sg = I16_Wrapped(m, (void *)(&h.orderIdx)-(void *)(&h));
        sg->objType.of = TYPE_I32;
        Table_Set(cls->atts,
            Str_CstrRef(m, "orderIdx"), sg);

        sg = I16_Wrapped(m, (void *)(&h.id)-(void *)(&h));
        sg->objType.of = TYPE_UTIL;
        Table_Set(cls->atts,
            Str_CstrRef(m, "id"), sg);

        sg = I16_Wrapped(m, (void *)(&h.key)-(void *)(&h));
        Table_Set(cls->atts, Str_CstrRef(m, "key"), sg);
        Table_Set(cls->atts, Str_CstrRef(m, "@"), sg);

        sg = I16_Wrapped(m, (void *)(&h.value)-(void *)(&h));
        Table_Set(cls->atts, Str_CstrRef(m, "value"), sg);
        Table_Set(cls->atts, Str_CstrRef(m, "_"), sg);

        r |= Lookup_Add(m, lk, TYPE_HASHED, (void *)cls);

        /* span */
        Span p;
        cls = ClassDef_Make(m);

        sg = NULL;
        sg = I16_Wrapped(m, (void *)(&p.nvalues)-(void *)(&p));
        sg->objType.of = TYPE_I32;
        Table_Set(cls->atts,
            Str_CstrRef(m, "count"), sg);

        sg = I16_Wrapped(m, (void *)(&p.max_idx)-(void *)(&p));
        sg->objType.of = TYPE_I32;
        Table_Set(cls->atts,
            Str_CstrRef(m, "maxIdx"), sg);

        r |= Lookup_Add(m, lk, TYPE_SPAN, (void *)cls);

        /* table */
        Table tbl;
        cls = ClassDef_Make(m);

        sg = NULL;
        sg = I16_Wrapped(m, (void *)(&tbl.nvalues)-(void *)(&tbl));
        sg->objType.of = TYPE_I32;
        Table_Set(cls->atts,
            Str_CstrRef(m, "count"), sg);

        r |= Lookup_Add(m, lk, TYPE_TABLE, (void *)cls);
    }

    if(r == READY){
        r |= NOOP;
    }

    return r;
}
