#include <external.h>
#include <caneka.h>

static boolean _init = FALSE;

static Abstract *OrdTable_ByKey(MemCh *m, FetchTarget *fg, Abstract *data, Abstract *source){
    OrdTable *otbl = (OrdTable *)as(data, TYPE_ORDTABLE);
    Hashed *h = OrdTable_Get(otbl, (Abstract *)fg->key);
    if(h != NULL){
        return h->value;
    }
    return NULL;
}

static Abstract *OrdTable_ByIdx(MemCh *m, FetchTarget *fg, Abstract *data, Abstract *source){
    OrdTable *otbl = (OrdTable *)as(data, TYPE_ORDTABLE);
    return Span_Get(otbl->order, fg->idx);
    return NULL;
}

static Abstract *OrdTable_GetIter(MemCh *m, FetchTarget *fg, Abstract *data, Abstract *source){
    OrdTable *otbl = (OrdTable *)as(data, TYPE_ORDTABLE);
    return (Abstract *)Iter_Make(m, otbl->order);
}

status Navigate_ClsInit(MemCh *m){
    status r = READY;
    Lookup *lk = NULL;
    if(!_init){
        _init = TRUE;
        ClassDef *cls = ClassDef_Make(m);
        cls = ClassDef_Make(m);
        cls->byKey = OrdTable_ByKey;
        cls->byIdx = OrdTable_ByIdx;
        cls->getIter = OrdTable_ByIdx;
        OrdTable otbl;
        Table_Set(cls->atts, (Abstract *)Str_CstrRef(m, "tbl"),
            (Abstract *)I16_Wrapped(m, (void *)(&otbl.tbl)-(void *)(&otbl)));
        Table_Set(cls->atts, (Abstract *)Str_CstrRef(m, "order"),
            (Abstract *)I16_Wrapped(m, (void *)(&otbl.order)-(void *)(&otbl)));
        lk = ClassLookup;
        r |= Lookup_Add(m, ClassLookup, TYPE_ORDTABLE, (void *)cls);
    }
    if(r == READY){
        r |= NOOP;
    }
    return r;
}
