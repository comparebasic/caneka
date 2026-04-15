#include <external.h>
#include <caneka.h>

StrVec *StrVec_Snip(MemCh *m, Span *sns, Cursor *_curs){
    DebugStack_Push(sns, TYPE_SPAN);

    StrVec *v = StrVec_Make(m);
    Cursor *curs = Cursor_Copy(m, _curs);
    i64 total = SnipSpan_Total(sns, 0)-1;
    Cursor_Decr(curs, total);
    void *args[] = {
        I64_Wrapped(m, total),
        curs,
        NULL
    };

    Iter it;
    Iter_Init(&it, sns);
    while((Iter_Next(&it) & END) == 0){
        Snip *sn = (Snip *)it.value;
        if(sn->type.state & SNIP_STR_BOUNDRY){
            continue;
        }else if(sn->type.state & SNIP_CONTENT){
            void *args[] = {
                Str_Ref(m, curs->ptr, sn->length, sn->length+1, 0),
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

status SnipSpan_SetAll(Span *sns, word flags){
    i64 total = SnipSpan_Total(sns, 0);
    Span_Wipe(sns);
    Snip *sn = Snip_Make(sns->m);
    sn->length = total;
    sn->type.state = flags;
    return Span_Add(sns, sn);
}

status SnipSpan_Set(MemCh *m, Span *sns, i32 length, word flags){
    Snip *sn = NULL;
    if(length == 0){
        return NOOP;
    }

    Iter it;
    Iter_Init(&it, sns);
    while((Iter_Prev(&it) & END) == 0 && length > 0){
        Snip *sn = (Snip *)it.value;

        i32 amount = min(sn->length, length);
        sn->length -= amount;
        if(amount > 0){
            Snip *sn = Snip_Make(m);
            sn->length = amount;
            sn->type.state = flags;
            Span_Insert(sns, it.idx+1, sn);
            length -= amount;
        }
    }

    return SUCCESS;
}

status SnipSpan_AddFrom(Span *sns, i32 length, word flags){
    Snip _sn = {.type = {TYPE_SNIP, flags}, length};
    return Span_Add(sns, Snip_From(sns->m, &_sn));
}

status SnipSpan_Add(Span *sns, Snip *sn){
    return Span_Add(sns, Snip_From(sns->m, sn));
}

status SnipSpan_Remove(Span *sns, i32 length){
    Iter it;
    Iter_Init(&it, sns);
    while((Iter_Prev(&it) & END) == 0 && length > 0){
        Snip *sn = (Snip *)it.value; 
        if(sn->length <= length){
            sn->length -= length;
            length = 0;
        }else{
            length -= sn->length;
            sn->length = 0;
        }
    }
    Snip *sn = Snip_Make(sns->m);
    sn->type.state |= SNIP_GAP;
    sn->type.state |= length;
    return SnipSpan_Add(sns, sn);
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
