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

status SnipSpan_SetAll(Span *sns, word flags){
    i64 total = SnipSpan_Total(sns, 0);
    Span_Wipe(sns);
    Snip *sn = Snip_Make(sns->m);
    sn->length = total;
    sn->type.state = flags;
    return Span_Add(sns, (Abstract *)sn);
}

status SnipSpan_Set(MemCh *m, Span *sns, i32 pos, i32 length, word flags){
    if(length == 0){
        return NOOP;
    }
    Iter it;
    Iter_Init(&it, sns);
    i32 position = 0;
    Snip *sn = NULL;
    while((Iter_Next(&it) & END) == 0){
        sn = (Snip *)it.value;
        if(position + sn->length > pos){
            break;
        }
        position += sn->length;
    }

    if(sn->length == 0 ){
        it.idx++;
    }else if((sn->type.state & flags) == flags){
        i32 remove = pos - position;
        if(sn->length > remove + length){
            return SUCCESS;
        }else{
            length -= (sn->length - remove);
        }
    }else{
        i32 remaining = pos - position;
        i32 removed = 0;
        if(sn->length > remaining){
            removed = sn->length - remaining;
            sn->length = remaining;
        }else{
            sn->length = 0;
            removed = sn->length;
        }
        i32 amount = min(length, removed);

        Snip *sn = Snip_Make(m);
        sn->length = amount;
        sn->type.state = flags;
        Span_Insert(sns, it.idx+1, (Abstract *)sn);
        length -= amount;
        it.idx++;
    }

    Iter_Setup(&it, sns, SPAN_OP_GET, it.idx);
    it.type.state |= PROCESSING;
    Iter_Query(&it);
    while((Iter_Next(&it) & END) == 0 && length > 0){
        Snip *sn = (Snip *)it.value;

        Abstract *args[] = {
            (Abstract *)I32_Wrapped(OutStream->m, it.idx),
            (Abstract *)it.value,
            NULL
        };
        Out("^c.Iter_Next below: $:&\n", args);

        i32 amount = min(sn->length, length);
        sn->length -= amount;
        if(amount > 0){
            Snip *sn = Snip_Make(m);
            sn->length = amount;
            sn->type.state = flags;
            Span_Insert(sns, it.idx+1, (Abstract *)sn);
            it.idx++;
            length -= amount;
        }
    }

    return SUCCESS;
}


status SnipSpan_Add(Span *sns, Snip *sn){
    return Span_Add(sns, (Abstract *)Snip_From(sns->m, sn));
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
