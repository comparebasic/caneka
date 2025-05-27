#include <external.h>
#include <caneka.h>

i64 HKey_Print(Stream *sm, Abstract *a, cls type, word flags){
    HKey *hk = (HKey *)as(a, TYPE_HKEY);
    if(flags & (MORE|DEBUG)){
        Abstract *args[] = {
            (Abstract *)I32_Wrapped(sm->m, hk->idx), 
            (Abstract *)I32_Wrapped(sm->m, hk->idx),
            (Abstract *)I8_Wrapped(sm->m, hk->dim), 
            (Abstract *)I8_Wrapped(sm->m, hk->pos),
            NULL
        };
        return Fmt(sm, "HKey<$/$ ^D.$^d.dim ^D.$^d.pos>", args);
    }else{
        return ToStream_NotImpl(sm, a, type, flags);
    }
}

i64 Array_Print(Stream *sm, Abstract *a, cls type, word flags){
    Abstract **arr = (Abstract **)a;
    i64 total = 0;
    boolean first = TRUE;
    while(*arr != NULL){
        if(first){
            first = FALSE;
        }else if(flags & MORE){
            total += Stream_Bytes(sm, (byte *)",", 1);
        }
        total += ToS(sm, *arr, 0, flags);
        arr++;
    }
    return total;
}

i64 Hashed_Print(Stream *sm, Abstract *a, cls type, word flags){
    Hashed *h = (Hashed *)as(a, TYPE_HASHED);
    if(flags & DEBUG){
        Single *wid = I64_Wrapped(sm->m, h->id);
        wid->type.state |= FMT_TYPE_BITS;
        Single *val = Ptr_Wrapped(sm->m, h->value, 0);
        Abstract *args[] = {
            (Abstract *)I32_Wrapped(sm->m, h->idx), 
            (Abstract *)wid, 
            h->item, 
            (Abstract *)val, 
            NULL
        };
        return Fmt(sm, "H<$ $/$ -> $>", args);
    }else if(flags & MORE){
        Single *val = Ptr_Wrapped(sm->m, h->value, 0);
        Abstract *args[] = {
            (Abstract *)I32_Wrapped(sm->m, h->idx), 
            h->item, 
            (Abstract *)val, 
            NULL
        };
        return Fmt(sm, "H<$ $ -> $>", args);
    }else{
        return ToStream_NotImpl(sm, a, type, flags);
    }
}

i64 Lookup_Print(Stream *sm, Abstract *a, cls type, word flags){
    Lookup *lk = (Lookup *)as(a, TYPE_LOOKUP);
    i64 total = 0;
    if(flags & (MORE|DEBUG)){
        i64 total = 0;
        Abstract *args[] = {
            (Abstract *)I32_Wrapped(sm->m, lk->offset),
            NULL,
        };
        total += Fmt(sm, "Lk<^D.$^d.offset values[", args);
        Iter it;
        Iter_Init(&it, lk->values);
        while((Iter_Next(&it) & END) == 0){
            if(it.value != NULL){
                /*
                Single *val = Ptr_Wrapped(sm->m, it.value, 0);
                val->type.state |= DEBUG;
                */
                Abstract *args[] = {
                    (Abstract *)I32_Wrapped(sm->m, it.idx+lk->offset),
                    (Abstract *)Type_ToStr(sm->m, it.idx+lk->offset),
                    NULL
                };
                total += Fmt(sm, "$/$", args);
                if((it.type.state & FLAG_ITER_LAST) == 0){
                    Stream_Bytes(sm, (byte *)", ", 2);
                }
            }
        }
        total += Fmt(sm, "]>", NULL);
    }else{
        total += ToStream_NotImpl(sm, a, type, flags);
    }
    return total;
}

i64 Table_Print(Stream *sm, Abstract *a, cls type, word flags){
    Table *tbl = (Table *)a;
    if((flags & (DEBUG|MORE)) == 0){
        return ToStream_NotImpl(sm, a, type, flags);
    }else{
        i64 total = 0;
        if(flags & DEBUG){
            Abstract *args[] = {
                (Abstract *)StreamTask_Make(sm->m, NULL, (Abstract *)tbl, ToS_FlagLabels),
                (Abstract *)I32_Wrapped(sm->m, tbl->nvalues),
                NULL
            };
            total += Fmt(sm, "Tbl<$ $nvalues ", args);
        }else{
            total += Stream_Bytes(sm, (byte *)"{", 1);
        }
        Iter it;
        Iter_Init(&it, tbl);
        while((Iter_Next(&it) & END) == 0){
            if(it.value != NULL){
                Hashed *h = (Hashed *)it.value;
                total += ToS(sm, h->item, 0, flags|MORE);
                total += Stream_Bytes(sm, (byte *)"=", 1);
                total += ToS(sm, h->value, 0, flags|MORE);
                if((it.type.state & FLAG_ITER_LAST) == 0){
                    if(flags & DEBUG){
                        total += Stream_Bytes(sm, (byte *)", ", 2);
                    }else{
                        total += Stream_Bytes(sm, (byte *)",", 1);
                    }
                }
            }
        }
        if(flags & DEBUG){
            total += Stream_Bytes(sm, (byte *)">", 1);
        }else{
            total += Stream_Bytes(sm, (byte *)"}", 1);
        }
        return total;
    }
}

status Sequence_ToSInit(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_LOOKUP, (void *)Lookup_Print);
    r |= Lookup_Add(m, lk, TYPE_TABLE, (void *)Table_Print);
    r |= Lookup_Add(m, lk, TYPE_HKEY, (void *)HKey_Print);
    r |= Lookup_Add(m, lk, TYPE_HASHED, (void *)Hashed_Print);
    r |= Lookup_Add(m, lk, TYPE_ARRAY, (void *)Array_Print);
    return r;
}
