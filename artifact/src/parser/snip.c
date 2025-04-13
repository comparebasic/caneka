#include <external.h>
#include <caneka.h>

StrVec *StrVec_Snip(MemCh *m, Span *sns, Cursor *curs){
    DebugStack_Push(sns, TYPE_STRSNIP_STRING);
    StrVec *ret = StrVec_Make(m);

    Iter it;
    Iter_Init(&it, sns);
    i32 pos = 0;
    Iter_Reset(&curs->it); 
    while((Iter_Next(&it) & END) == 0){
        StrSnip sn = (StrSnip)((util)Iter_Get(&it));
        if(s.type.state & STRSNIP_CONTENT){
            StrVec_Copy(v, ret, pos, pos+sn.length);
        }
        pos += sn.length;
    }

    return ret;
}

status SnipSpan_Add(Span *sns, Snip *sn){
    return Span_Add(sns, Snip_From(sns->m, sn));
}

i64 SnipSpan_Total(Span *sns, word flags){
    Iter it;
    Iter_Init(&it, sns);
    i64 total = 0;
    while((Iter_Next(&it) & END) == 0){
        Snip *sn = (Snip *)it.value;
        if(flags == 0 || (sn->type.state & flags)){
            total += sn->length;
        }
    }

    return total;
}

Snip *Snip_From(MemCh *m, Snip *_sn){
    Snip *sn = Snip_Make(m);
    memcpy(sn, _sn, sizeof(Snip));
    return sn;
}

Snip *Snip_Make(MemCh *m){
    Snip *sn = (Snip *)MemCh_Alloc(m, sizeof(Snip));
    sn->type.of = TYPE_SNIP;
    return sn;
}
