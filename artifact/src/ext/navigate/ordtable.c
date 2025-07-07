#include <external.h>
#include <caneka.h>

Hashed *OrdTable_Set(OrdTable *otbl, Abstract *key, Abstract *value){
    otbl->type.state &= ~OUTCOME_FLAGS;

    Hashed *h = Table_SetHashed(otbl->tbl, key, value);
    otbl->type.state |= otbl->tbl->type.state & OUTCOME_FLAGS;
    if(h != NULL){
        Span_Add(otbl->order, (Abstract *)h);
        if(otbl->order->type.state & SUCCESS){
            h->orderIdx = otbl->order->max_idx;
        }
        otbl->type.state |= otbl->order->type.state & OUTCOME_FLAGS; 
    }
    return h;
}

Hashed *OrdTable_Get(OrdTable *tbl, Abstract *key){
    return Table_GetHashed(tbl->tbl, key);
}

Hashed *OrdTable_GetByIdx(OrdTable *tbl, i32 idx){
    return (Hashed *)Span_Get(tbl->order, idx);
}

OrdTable *OrdTable_Make(MemCh *m){
    OrdTable *otbl = (OrdTable *)MemCh_Alloc(m, sizeof(OrdTable));
    otbl->type.state |= TYPE_ORDTABLE;
    otbl->tbl = Table_Make(m);
    otbl->order = Span_Make(m);
    return otbl;
}
