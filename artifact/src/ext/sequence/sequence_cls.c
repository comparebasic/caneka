#include <external.h>
#include <caneka.h>

static boolean _init = FALSE;

status Sequence_ClassInit(MemCh *m, Lookup *lk){
    status r = READY;

    if(!_init){
        _init = TRUE;
        Hashed h;
        ClassDef *cls = ClassDef_Make(m);

        Single *sg = NULL;
        sg = I16_Wrapped(m, (void *)(&h.idx)-(void *)(&h));
        sg->objType.of = TYPE_I32;
        Table_Set(cls->atts,
            (Abstract *)Str_CstrRef(m, "idx"), (Abstract *)sg);

        sg = I16_Wrapped(m, (void *)(&h.orderIdx)-(void *)(&h));
        sg->objType.of = TYPE_I32;
        Table_Set(cls->atts,
            (Abstract *)Str_CstrRef(m, "orderIdx"), (Abstract *)sg);

        sg = I16_Wrapped(m, (void *)(&h.id)-(void *)(&h));
        sg->objType.of = TYPE_UTIL;
        Table_Set(cls->atts,
            (Abstract *)Str_CstrRef(m, "id"), (Abstract *)sg);

        sg = I16_Wrapped(m, (void *)(&h.key)-(void *)(&h));
        Table_Set(cls->atts, (Abstract *)Str_CstrRef(m, "key"), (Abstract *)sg);
        Table_Set(cls->atts, (Abstract *)Str_CstrRef(m, "@"), (Abstract *)sg);

        sg = I16_Wrapped(m, (void *)(&h.value)-(void *)(&h));
        Table_Set(cls->atts, (Abstract *)Str_CstrRef(m, "value"), (Abstract *)sg);
        Table_Set(cls->atts, (Abstract *)Str_CstrRef(m, "_"), (Abstract *)sg);

        r |= Lookup_Add(m, lk, TYPE_HASHED, (void *)cls);
    }

    if(r == READY){
        r |= NOOP;
    }

    return r;
}
