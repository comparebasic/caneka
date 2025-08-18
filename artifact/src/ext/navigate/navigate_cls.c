#include <external.h>
#include <caneka.h>

static boolean _init = FALSE;

static Abstract *PathTable_ByKey(MemCh *m, FetchTarget *fg, Abstract *data, Abstract *source){
    PathTable *pt = (PathTable *)as(data, TYPE_PATHTABLE);
    Hashed *h = PathTable_Get(pt, (Abstract *)fg->key);
    if(h != NULL){
        return h->value;
    }
    return NULL;
}

static Abstract *PathTable_ByIdx(MemCh *m, FetchTarget *fg, Abstract *data, Abstract *source){
    PathTable *pt = (PathTable *)as(data, TYPE_PATHTABLE);
    return Span_Get(pt->order, fg->idx);
    return NULL;
}

static Abstract *PathTable_GetIter(MemCh *m, FetchTarget *fg, Abstract *data, Abstract *source){
    PathTable *pt = (PathTable *)as(data, TYPE_PATHTABLE);
    return (Abstract *)Iter_Make(m, pt->order);
}

status Navigate_ClsInit(MemCh *m){
    status r = READY;
    Lookup *lk = NULL;
    if(!_init){
        _init = TRUE;
        ClassDef *cls = ClassDef_Make(m);
        cls = ClassDef_Make(m);
        cls->byKey = PathTable_ByKey;
        cls->byIdx = PathTable_ByIdx;
        cls->getIter = PathTable_GetIter;
        PathTable pt;
        Table_Set(cls->atts, (Abstract *)Str_CstrRef(m, "tbl"),
            (Abstract *)I16_Wrapped(m, (void *)(&pt.tbl)-(void *)(&pt)));
        Table_Set(cls->atts, (Abstract *)Str_CstrRef(m, "order"),
            (Abstract *)I16_Wrapped(m, (void *)(&pt.order)-(void *)(&pt)));
        lk = ClassLookup;
        r |= Lookup_Add(m, ClassLookup, TYPE_PATHTABLE, (void *)cls);
    }
    if(r == READY){
        r |= NOOP;
    }
    return r;
}
