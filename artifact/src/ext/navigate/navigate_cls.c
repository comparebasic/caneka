#include <external.h>
#include <caneka.h>

static ClassDef *ordTableCls = NULL;

static Abstract *OrdTable_ByKey(FetchTarget *fg, Abstract *data, Abstract *source){
    OrdTable *tbl = as(data, TYPE_ORDTABLE);
    if(fg->type.state & FETCH_TARGET_KEY){
        return OrdTable_Get(data, fg->val.key);
    }
    return NULL;
}

static Abstract *OrdTable_ByIdx(FetchTarget *fg, Abstract *data, Abstract *source){
    OrdTable *tbl = as(data, TYPE_ORDTABLE);
    if(fg->type.state & FETCH_TARGET_IDX){
        return Span_Get(tbl->order, fg->val.idx);
    }
    return NULL
}

status Navigate_ClsInit(MemCh *m, Lookup *lk){
    status r = READY;
    if(ordTableCls == NULL){
        ordTableCls = ClassDef_Make(m);
        ordTableCls->byKey = OrdTable_ByKey;
        ordTableCls->byIdx = OrdTable_ByIdx;
        OrdTable otbl;
        Table_Set(ordTableCls->attsTbl, (Abstract *)Str_CstrRef(m, "tbl"),
            (Abstract *)I16_Wrapped(m, (void *)(&otbl.tbl)-(void *)(&otbl)));
        Table_Set(ordTableCls->attsTbl, (Abstract *)Str_CstrRef(m, "order"),
            (Abstract *)I16_Wrapped(m, (void *)(&otbl.order)-(void *)(&otbl)));
        r |= Lookup_Add(m, lk, TYPE_ORDTABLE, (void *)ordTableCls);
    }
    if(r == READY){
        r |= NOOP;
    }
    return r;
}
