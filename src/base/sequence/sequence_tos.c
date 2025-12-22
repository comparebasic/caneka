#include <external.h>
#include "base_module.h"

status HKey_Print(Buff *bf, void *a, cls type, word flags){
    HKey *hk = (HKey *)as(a, TYPE_HKEY);
    if(flags & (MORE|DEBUG)){
        void *args[] = {
            I32_Wrapped(bf->m, hk->idx), 
            I8_Wrapped(bf->m, hk->dim), 
            I8_Wrapped(bf->m, hk->pos),
            NULL
        };
        return Fmt(bf, "HKey<idx^D.$^d. ^D.$^d.dim ^D.$^d.pos>", args);
    }else{
        return ToStream_NotImpl(bf, a, type, flags);
    }
}

status CstrArray_Print(Buff *bf, void *a, cls type, word flags){
    printf("CstrArray Print\n");
    fflush(stdout);
    byte **arr = (byte **)a;
    boolean first = TRUE;
    Str s = {
        .type = {TYPE_STR, STRING_CONST},
        .length = 0,
        .alloc = 0,
        .bytes = NULL,
    };
    while(*arr != NULL){
        if(first){
            first = FALSE;
        }else if(flags & MORE){
            Buff_AddBytes(bf, (byte *)", ", 2);
        }
        s.bytes = *arr;
        s.length = s.alloc = strlen((char *)*arr);
        Buff_Add(bf, &s);
        arr++;
    }
    return ZERO;
}

status Array_Print(Buff *bf, void *a, cls type, word flags){
    printf("Array Print\n");
    fflush(stdout);
    void **arr = (void **)a;
    boolean first = TRUE;
    while(*arr != NULL){
        if(first){
            first = FALSE;
        }else if(flags & MORE){
            Buff_AddBytes(bf, (byte *)",", 1);
        }
        ToS(bf, *arr, 0, flags);
        arr++;
    }
    return SUCCESS;
}

status Hashed_Print(Buff *bf, void *a, cls type, word flags){
    Hashed *h = (Hashed *)as(a, TYPE_HASHED);
    if(flags & DEBUG){
        Single *wid = I64_Wrapped(bf->m, h->id);
        wid->type.state |= FMT_TYPE_BITS;
        Single *val = Ptr_Wrapped(bf->m, h->value, 0);
        void *args[] = {
            I32_Wrapped(bf->m, h->orderIdx), 
            I32_Wrapped(bf->m, h->idx), 
            wid, 
            h->key, 
            val, 
            (h->value != NULL ? Type_ToStr(bf->m, ((Abstract *)h->value)->type.of) : NULL),
            NULL
        };
        return Fmt(bf, "H<$,$ $/@ -> $/$>", args);
    }else if(flags & MORE){
        void *args[] = {
            I32_Wrapped(bf->m, h->idx), 
            h->key, 
            h->value, 
            NULL
        };
        return Fmt(bf, "H<$ @ -> @>", args);
    }else{
        return ToStream_NotImpl(bf, a, type, flags);
    }
}

status Lookup_Print(Buff *bf, void *a, cls type, word flags){
    Lookup *lk = (Lookup *)as(a, TYPE_LOOKUP);
    if(flags & (MORE|DEBUG)){
        void *args[] = {
            I32_Wrapped(bf->m, lk->offset),
            NULL,
        };
        Fmt(bf, "Lk<^D.$^d.offset values[", args);
        Iter it;
        Iter_Init(&it, lk->values);
        while((Iter_Next(&it) & END) == 0){
            if(it.value != NULL){
                void *args[] = {
                    I32_Wrapped(bf->m, it.idx+lk->offset),
                    Type_ToStr(bf->m, it.idx+lk->offset),
                    NULL
                };
                Fmt(bf, "$/$", args);
                if((it.type.state & LAST) == 0){
                    Buff_AddBytes(bf, (byte *)", ", 2);
                }
            }
        }
        Fmt(bf, "]>", NULL);
    }else{
        ToStream_NotImpl(bf, a, type, flags);
    }
    return SUCCESS;
}

status Table_Print(Buff *bf, void *a, cls type, word flags){
    Table *tbl = (Table *)a;
    if((flags & (DEBUG|MORE)) == 0){
        return ToStream_NotImpl(bf, a, type, flags);
    }else{
        if(flags & DEBUG){
            void *args[] = {
                Type_StateVec(bf->m, tbl->type.of, tbl->type.state),
                I32_Wrapped(bf->m, tbl->nvalues),
                NULL
            };
            Fmt(bf, "Tbl<$ ^D.$^d.nvalues ", args);
        }else{
            Buff_AddBytes(bf, (byte *)"{", 1);
        }
        Iter it;
        Iter_Init(&it, tbl);
        while((Iter_Next(&it) & END) == 0){
            Hashed *h = (Hashed *)Iter_Get(&it);;
            if(h != NULL){
                ToS(bf, h->key, 0, flags|MORE);
                Buff_AddBytes(bf, (byte *)"=", 1);
                ToS(bf, h->value, 0, flags|MORE);
                if((it.type.state & LAST) == 0){
                    if(flags & DEBUG){
                        Buff_AddBytes(bf, (byte *)", ", 2);
                    }else{
                        Buff_AddBytes(bf, (byte *)",", 1);
                    }
                }
            }
        }
        if(flags & DEBUG){
            Buff_AddBytes(bf, (byte *)">", 1);
        }else{
            Buff_AddBytes(bf, (byte *)"}", 1);
        }
        return SUCCESS;
    }
}

status Sequence_ToSInit(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_LOOKUP, (void *)Lookup_Print);
    r |= Lookup_Add(m, lk, TYPE_TABLE, (void *)Table_Print);
    r |= Lookup_Add(m, lk, TYPE_HKEY, (void *)HKey_Print);
    r |= Lookup_Add(m, lk, TYPE_HASHED, (void *)Hashed_Print);
    r |= Lookup_Add(m, lk, TYPE_ARRAY, (void *)Array_Print);
    r |= Lookup_Add(m, lk, TYPE_CSTR_ARRAY, (void *)CstrArray_Print);
    return r;
}
