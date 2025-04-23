#include <external.h>
#include <caneka.h>

StrVec *StrVec_Snip(MemCh *m, Span *sns, Cursor *_curs){
    DebugStack_Push(sns, TYPE_SPAN);

    StrVec *v = StrVec_Make(m);
    Cursor *curs = Cursor_Copy(m, _curs);
    Cursor_Decr(curs, SnipSpan_Total(sns, 0));

    Iter it;
    Iter_Init(&it, sns);

    Iter_Reset(&curs->it);
    Iter_Next(&curs->it);
    Str *s = (Str *)curs->it.value;
    i64 pos;
    while((Iter_Next(&it) & END) == 0){
        Snip *sn = (Snip *)it.value;
        if(sn->type.state & SNIP_STR_BOUNDRY){
            curs->it.idx++;
            Iter_Query(&curs->it);
            s = (Str *)curs->it.value;
        }else if(sn->type.state & SNIP_GAP){
            pos = sn->length;
        }else if(sn->type.state & SNIP_CONTENT){
            StrVec_AddBytes(m, v, s->bytes+pos, sn->length);
            pos = 0;
        }
    }

    DebugStack_Pop();
    return v;
}

status SnipSpan_Add(Span *sns, Snip *sn){
    return Span_Add(sns, (Abstract *)Snip_From(sns->m, sn));
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
