#include <external.h>
#include <caneka.h>

StrVec *StrVec_Snip(MemCh *m, Span *sns, Cursor *_curs){
    DebugStack_Push(sns, TYPE_SPAN);

    StrVec *v = StrVec_Make(m);
    Cursor *curs = Cursor_Copy(m, _curs);
    i64 total = SnipSpan_Total(sns, 0)-1;
    Cursor_Decr(curs, total);
    Abstract *args[] = {
        (Abstract *)I64_Wrapped(m, total),
        (Abstract *)curs,
        NULL
    };

    Iter it;
    Iter_Init(&it, sns);
    while((Iter_Next(&it) & END) == 0){
        Snip *sn = (Snip *)it.value;
        if(sn->type.state & SNIP_STR_BOUNDRY){
            continue;
        }else if(sn->type.state & SNIP_CONTENT){
            Abstract *args[] = {
                (Abstract *)Str_Ref(m, curs->ptr, sn->length, sn->length+1, 0),
                NULL,
            };
            StrVec_AddBytes(m, v, curs->ptr, sn->length);
            Cursor_Incr(curs, sn->length);
        }else if(sn->type.state){
            Cursor_Incr(curs, sn->length);
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
