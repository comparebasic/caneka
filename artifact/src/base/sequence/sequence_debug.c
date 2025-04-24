#include <external.h>
#include <caneka.h>

i64 HKey_Print(Stream *sm, Abstract *a, cls type, boolean extended){
    HKey *hk = (HKey *)as(a, TYPE_HKEY);
    Abstract *args[] = {
        (Abstract *)I32_Wrapped(sm->m, hk->idx), 
        (Abstract *)I32_Wrapped(sm->m, hk->idx),
        (Abstract *)I8_Wrapped(sm->m, hk->dim), 
        (Abstract *)I8_Wrapped(sm->m, hk->pos),
        NULL
    };
    return Fmt(sm, "HKey<$/$ ^D.$^d.dim ^D.$^d.pos>", args);
}

i64 Hashed_Print(Stream *sm, Abstract *a, cls type, boolean extended){
    Hashed *h = (Hashed *)as(a, TYPE_HASHED);
    Single *wid = I64_Wrapped(sm->m, h->id);
    wid->type.state |= FMT_TYPE_BITS;
    Single *val = Ptr_Wrapped(sm->m, h->value);
    val->type.state |= DEBUG;
    if(extended){
        Abstract *args[] = {
            (Abstract *)I32_Wrapped(sm->m, h->idx), 
            (Abstract *)wid, 
            h->item, 
            (Abstract *)val, 
            NULL
        };
        return Fmt(sm, "H<$ $/$ -> $>", args);
    }else{
        Abstract *args[] = {
            (Abstract *)I32_Wrapped(sm->m, h->idx), 
            h->item, 
            (Abstract *)val, 
            NULL
        };
        return Fmt(sm, "H<$ $ -> $>", args);
    }
}

i64 Lookup_Print(Stream *sm, Abstract *a, cls type, boolean extended){
    Lookup *lk = (Lookup *)as(a, TYPE_LOOKUP);
    i64 total = 0;
    total += Fmt(sm, "Lk<", NULL);
    Iter it;
    Iter_Init(&it, lk->values);
    while((Iter_Next(&it) & END) == 0){
        if(it.value != NULL){
            Single *val = Ptr_Wrapped(sm->m, it.value);
            val->type.state |= DEBUG;
            Abstract *args[] = {
                (Abstract *)I32_Wrapped(sm->m, it.idx),
                (Abstract *)val,
                NULL
            };
            total += Fmt(sm, "$ -> $", args);
            if((it.type.state & FLAG_ITER_LAST) == 0){
                Stream_To(sm, (byte *)", ", 2);
            }
        }
    }
    total += Fmt(sm, ">", NULL);
    return total;
}

status SequenceDebug_Init(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_LOOKUP, (void *)Lookup_Print);
    r |= Lookup_Add(m, lk, TYPE_TABLE, (void *)Span_Print);
    r |= Lookup_Add(m, lk, TYPE_HKEY, (void *)HKey_Print);
    r |= Lookup_Add(m, lk, TYPE_HASHED, (void *)Hashed_Print);
    return r;
}
