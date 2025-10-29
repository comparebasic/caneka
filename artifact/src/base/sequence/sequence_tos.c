#include <external.h>
#include <caneka.h>

i64 HKey_Print(Buff *bf, Abstract *a, cls type, word flags){
    HKey *hk = (HKey *)as(a, TYPE_HKEY);
    if(flags & (MORE|DEBUG)){
        Abstract *args[] = {
            (Abstract *)I32_Wrapped(bf->m, hk->idx), 
            (Abstract *)I8_Wrapped(bf->m, hk->dim), 
            (Abstract *)I8_Wrapped(bf->m, hk->pos),
            NULL
        };
        return Fmt(bf, "HKey<idx^D.$^d. ^D.$^d.dim ^D.$^d.pos>", args);
    }else{
        return ToStream_NotImpl(bf, a, type, flags);
    }
}

i64 Array_Print(Buff *bf, Abstract *a, cls type, word flags){
    Abstract **arr = (Abstract **)a;
    i64 total = 0;
    boolean first = TRUE;
    while(*arr != NULL){
        if(first){
            first = FALSE;
        }else if(flags & MORE){
            total += Buff_Bytes(bf, (byte *)",", 1);
        }
        total += ToS(bf, *arr, 0, flags);
        arr++;
    }
    return total;
}

i64 Hashed_Print(Buff *bf, Abstract *a, cls type, word flags){
    Hashed *h = (Hashed *)as(a, TYPE_HASHED);
    if(flags & DEBUG){
        Single *wid = I64_Wrapped(bf->m, h->id);
        wid->type.state |= FMT_TYPE_BITS;
        Single *val = Ptr_Wrapped(bf->m, h->value, 0);
        Abstract *args[] = {
            (Abstract *)I32_Wrapped(bf->m, h->orderIdx), 
            (Abstract *)I32_Wrapped(bf->m, h->idx), 
            (Abstract *)wid, 
            h->key, 
            (Abstract *)val, 
            (Abstract *)(h->value != NULL ? Type_ToStr(bf->m, h->value->type.of) : NULL),
            NULL
        };
        return Fmt(bf, "H<$,$ $/@ -> $/$>", args);
    }else if(flags & MORE){
        Abstract *args[] = {
            (Abstract *)I32_Wrapped(bf->m, h->idx), 
            h->key, 
            (Abstract *)h->value, 
            NULL
        };
        return Fmt(bf, "H<$ @ -> @>", args);
    }else{
        return ToStream_NotImpl(bf, a, type, flags);
    }
}

i64 Lookup_Print(Buff *bf, Abstract *a, cls type, word flags){
    Lookup *lk = (Lookup *)as(a, TYPE_LOOKUP);
    i64 total = 0;
    if(flags & (MORE|DEBUG)){
        i64 total = 0;
        Abstract *args[] = {
            (Abstract *)I32_Wrapped(bf->m, lk->offset),
            NULL,
        };
        total += Fmt(bf, "Lk<^D.$^d.offset values[", args);
        Iter it;
        Iter_Init(&it, lk->values);
        while((Iter_Next(&it) & END) == 0){
            if(it.value != NULL){
                /*
                Single *val = Ptr_Wrapped(bf->m, it.value, 0);
                val->type.state |= DEBUG;
                */
                Abstract *args[] = {
                    (Abstract *)I32_Wrapped(bf->m, it.idx+lk->offset),
                    (Abstract *)Type_ToStr(bf->m, it.idx+lk->offset),
                    NULL
                };
                total += Fmt(bf, "$/$", args);
                if((it.type.state & LAST) == 0){
                    Buff_Bytes(bf, (byte *)", ", 2);
                }
            }
        }
        total += Fmt(bf, "]>", NULL);
    }else{
        total += ToStream_NotImpl(bf, a, type, flags);
    }
    return total;
}

i64 Table_Print(Buff *bf, Abstract *a, cls type, word flags){
    Table *tbl = (Table *)a;
    if((flags & (DEBUG|MORE)) == 0){
        return ToStream_NotImpl(bf, a, type, flags);
    }else{
        i64 total = 0;
        if(flags & DEBUG){
            Abstract *args[] = {
                (Abstract *)Type_StateVec(bf->m, tbl->type.of, tbl->type.state),
                (Abstract *)I32_Wrapped(bf->m, tbl->nvalues),
                NULL
            };
            total += Fmt(bf, "Tbl<$ ^D.$^d.nvalues ", args);
        }else{
            total += Buff_Bytes(bf, (byte *)"{", 1);
        }
        Iter it;
        Iter_Init(&it, tbl);
        while((Iter_Next(&it) & END) == 0){
            if(it.value != NULL){
                Hashed *h = (Hashed *)it.value;
                total += ToS(bf, h->key, 0, flags|MORE);
                total += Buff_Bytes(bf, (byte *)"=", 1);
                total += ToS(bf, h->value, 0, flags|MORE);
                if((it.type.state & LAST) == 0){
                    if(flags & DEBUG){
                        total += Buff_Bytes(bf, (byte *)", ", 2);
                    }else{
                        total += Buff_Bytes(bf, (byte *)",", 1);
                    }
                }
            }
        }
        if(flags & DEBUG){
            total += Buff_Bytes(bf, (byte *)">", 1);
        }else{
            total += Buff_Bytes(bf, (byte *)"}", 1);
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
