#include <external.h>
#include <caneka.h>

static ClassDef *ordTableCls = NULL;

static Abstract *OrdTable_ByKey(FetchTarget *fg, Abstract *data, Abstract *source){
    OrdTable *otbl = (OrdTable *)as(data, TYPE_ORDTABLE);
    if(fg->type.state & FETCH_TARGET_KEY){
        Hashed *h = OrdTable_Get(otbl, (Abstract *)fg->key);
        if(h != NULL){
            return h->value;
        }
    }
    return NULL;
}

static Abstract *OrdTable_ByIdx(FetchTarget *fg, Abstract *data, Abstract *source){
    OrdTable *otbl = (OrdTable *)as(data, TYPE_ORDTABLE);
    if(fg->type.state & FETCH_TARGET_IDX){
        return Span_Get(otbl->order, fg->idx);
    }
    return NULL;
}

status Navigate_ClsInit(MemCh *m){
    status r = READY;
    Lookup *lk = NULL;
    if(ordTableCls == NULL){
        ordTableCls = ClassDef_Make(m);
        ordTableCls->byKey = OrdTable_ByKey;
        ordTableCls->byIdx = OrdTable_ByIdx;
        OrdTable otbl;
        Table_Set(ordTableCls->attsTbl, (Abstract *)Str_CstrRef(m, "tbl"),
            (Abstract *)I16_Wrapped(m, (void *)(&otbl.tbl)-(void *)(&otbl)));
        Table_Set(ordTableCls->attsTbl, (Abstract *)Str_CstrRef(m, "order"),
            (Abstract *)I16_Wrapped(m, (void *)(&otbl.order)-(void *)(&otbl)));
        lk = ClassLookup;
        r |= Lookup_Add(m, lk, TYPE_ORDTABLE, (void *)ordTableCls);
    }
    if(r == READY){
        r |= NOOP;
    }
    return r;
}
