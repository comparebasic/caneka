#include <external.h>
#include <caneka.h>

static status Kve_Out(MemCtx *m, Hashed *h, OutFunc out){
    status r = READY;
    out(m, (String *)h->item, NULL);
    if(h->value != NULL && h->item != h->value){
        r |= out(m, String_Make(m, bytes(": ")), NULL);
        r |= out(m, String_FromAbs(m, h->value), NULL);
    }
    r |= out(m, String_Make(m, bytes("; ")), NULL);
    return r;
}

status Kve_OutFromTable(MemCtx *m, Span *p, String *first, OutFunc out){
    status r = READY;
    Span *tbl = (Span *)asIfc(p, TYPE_TABLE);
    int kveIdx = -1;
    int endIdx = -1;

    Iter it;
    if(p->type.of == TYPE_ORDERED_TABLE){
        OrderedTable *op = (OrderedTable *)p;
        Iter_Init(&it, op->order);
    }else{
        int kveIdx = Table_GetIdx(tbl, (Abstract *)first);
        int endIdx = Table_GetIdx(tbl, (Abstract *)String_Make(m, bytes("end")));
        if(kveIdx != -1){
            Hashed *h = (Hashed *)Span_Get(tbl, kveIdx);
            r |= Kve_Out(m, h, out);
        }
        Iter_Init(&it, p);
    }
    while((Iter_Next(&it) & END) == 0){
        if(it.idx == kveIdx || it.idx == endIdx){
            continue;
        }
        Hashed *h = (Hashed *)Iter_Get(&it);
        if(h != NULL){
            r |= Kve_Out(m, h, out);
        }
    }

    if(endIdx != -1){
        Hashed *h = (Hashed *)Span_Get(tbl, endIdx);
        r |= Kve_Out(m, h, out);
    }
    return r;
}
