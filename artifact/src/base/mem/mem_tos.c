#include <external.h>
#include <caneka.h>

static Str **iterLabels = NULL;

static inline i64 wsOut(Stream *sm, i8 dim){
    while(dim-- > 0){
        return Stream_Bytes(sm, (byte *)"    ", 4); 
    }
    return 0;
}

i64 MemPage_Print(Stream *sm, Abstract *a, cls type, word flags){
    i64 total = 0;
    MemPage *sl = (MemPage*)as(a, TYPE_MEMSLAB); 

    Abstract *args[] = {
        (Abstract *)I16_Wrapped(sm->m, sl->level),
        (Abstract *)I16_Wrapped(sm->m, sl->remaining),
        NULL
    };
    total = Fmt(sm, "Page<^D.$^d.level ^D.$^d.remaining", args);

    if(flags & DEBUG){
        Stream_Bytes(sm, (byte *)"[ ", 2);
        void *pos = sl;
        void *end = pos+PAGE_SIZE;
        pos += sizeof(MemPage)+(util)sl->remaining;

        size_t sz = MAX_BASE10+3;
        byte _digitBytes[sz];
        memset(_digitBytes, 0, sz);
        Str digit_s;
        Str_Init(&digit_s, _digitBytes, 0, sz);

        while(pos < end){
            Abstract *a = (Abstract *)pos;
            size_t osz = 0;
            if(a->type.of == TYPE_STRLIT){
                StrLit *sl = (StrLit *)a;
                osz = sizeof(RangeType)+sl->type.range;
            }else{
                i64 _n =  Lookup_GetRaw(SizeLookup, a->type.of);
                if(_n > 0){
                    osz = _n;
                }else{
                    Abstract *args[] = {
                        (Abstract *)Type_ToStr(sm->m, a->type.of),
                        NULL
                    };
                    Fatal(FUNCNAME, FILENAME, LINENUMBER,
                        "Unable to find size of type $", args);
                    return 0;
                }
            }


            Stream_Bytes(sm, (byte *)"\x1b[1m*", 5);
            Str_AddI64(&digit_s, (i64)sl);
            total += Stream_Bytes(sm, digit_s.bytes, digit_s.length);
            Stream_Bytes(sm, (byte *)"\x1b[22m/", 6);
            digit_s.length = 0;
            memset(_digitBytes, 0, sz);

            Str_AddI64(&digit_s, (i64)sz);
            total += Stream_Bytes(sm, digit_s.bytes, digit_s.length);
            Stream_Bytes(sm, (byte *)"bytes:", 6);
            digit_s.length = 0;
            memset(_digitBytes, 0, sz);

            if(a->type.of == TYPE_MEMSLAB){
                Stream_Bytes(sm, (byte *)"<self>", 6);
            }else if(a->type.of == TYPE_MEMCTX){
                Stream_Bytes(sm, (byte *)"<ctx>", 5);
            }else{
                total += ToS(sm, a, 0, flags);
            }

            if(osz < 0){
                break;
            }
            pos += osz;

            if(pos < end){
                Stream_Bytes(sm, (byte *)", ", 2);
            }
        }
        Stream_Bytes(sm, (byte *)"]", 1);
    }

    return Stream_Bytes(sm, (byte *)">", 1);
}

i64 MemCh_Print(Stream *sm, Abstract *a, cls type, word flags){
    MemCh *m = (MemCh*)as(a, TYPE_MEMCTX); 
    i64 total = 0;
    Abstract *args[] = {
        (Abstract *)I64_Wrapped(sm->m, m->it.p->nvalues),
        (Abstract *)MemCount_Wrapped(sm->m,  MemCh_Used(m, 0)),
        NULL
    };

    if(flags & MORE){
        total +=  Fmt(sm, "MemCh<$pages ^D.$^d.used [", args);
        Iter it;
        Iter_Init(&it, m->it.p);
        while((Iter_Next(&it) & END) == 0){
            ToS(sm, it.value, 0, flags|MORE);
            if((it.type.state & LAST) == 0){
                Stream_Bytes(sm, (byte *)",", 1);
            }
        }

        total +=  Stream_Bytes(sm, (byte *)"]>", 2);
    }else{
        return  Fmt(sm, "MemCh<used:$>", args);
    }
    return total;
}

i64 MemBook_Print(Stream *sm, Abstract *a, cls type, word flags){
    MemBook *cp = (MemBook*)as(a, TYPE_BOOK); 
    return 0;
}

i64 Span_Print(struct stream *sm, Abstract *a, cls type, word flags){
    Span *p = (Span*)as(a, TYPE_SPAN); 

    i64 total = 0;
    if(flags & (MORE|DEBUG)){
        Abstract *args[] = {
            (Abstract *)I32_Wrapped(sm->m, p->nvalues), 
            (Abstract *)I32_Wrapped(sm->m, p->max_idx), 
            (Abstract *)I8_Wrapped(sm->m, p->dims),
            NULL
        };
        total += Fmt(sm, "Span<^D.$^d.values/0..$/$dims [", args);
    }
    Iter it;
    Iter_Init(&it, p);
    while((Iter_Next(&it) & END) == 0){
        Abstract *a = it.value;
        if(a != NULL){
            Abstract *args[] = {
                (Abstract *)I32_Wrapped(sm->m, it.idx),
                NULL
            };
            if(flags & DEBUG){
                total += Stream_Bytes(sm, (byte *)"\n    ", 5);
            }
            if(flags & (MORE|DEBUG)){
                total += Fmt(sm, "$:", args);
            }
            Abstract *item = (Abstract *)it.value;
            if(p->type.state & FLAG_SPAN_RAW){
                total += Bits_Print(sm, (byte *)&(it.value), sizeof(void *), MORE);
            }else if(item != NULL && item->type.of == TYPE_MEMSLAB){
                total += ToS(sm, it.value, 0, (flags & ~DEBUG));
            }else{
                total += ToS(sm, it.value, 0, flags|MORE);
            }
            if((it.type.state & LAST) == 0 && (flags & MORE)){
                total += Stream_Bytes(sm, (byte *)", ", 2);
            }
        }
    }

    if(flags & (MORE|DEBUG)){
        total += Stream_Bytes(sm, (byte *)"]>", 2);
    }
    
    return total;
}

i64 Iter_Print(Stream *sm, Abstract *a, cls type, word flags){
    Iter *it = (Iter *)as(a, TYPE_ITER);
    i64 total = 0;
    if(flags & DEBUG){
        Abstract *args[] = {
            (Abstract *)StreamTask_Make(sm->m, NULL, (Abstract *)it, ToS_FlagLabels),
            (Abstract *)I32_Wrapped(sm->m, it->idx),
            (Abstract *)I32_Wrapped(sm->m, it->p->max_idx),
            (Abstract *)I8_Wrapped(sm->m, it->p->dims),
            NULL
        };
        total += Fmt(sm, "I<$ $ of $max/$dims\n", args);
        void *ptr = it->p->root;
        for(i8 i = it->p->dims; i >= 0; i--){
            if(it->stack[i] == NULL && (flags & (MORE|DEBUG))){
                Abstract *args[] = {
                    (Abstract *)I32_Wrapped(sm->m, i),
                    NULL
                };
                total += Fmt(sm, "  $: NULL\n", args);
            }else{
                i64 delta = 0;
                if(i > 0 && it->stack[i] != NULL){
                    delta = (it->stack[i] - ptr) / sizeof(void *);
                }
                Abstract *args[] = {
                    (Abstract *)I32_Wrapped(sm->m, i),
                    (Abstract *)Ptr_Wrapped(sm->m, ptr, 0),
                    (Abstract *)I32_Wrapped(sm->m, delta),
                    (Abstract *)I32_Wrapped(sm->m, it->stackIdx[i]),
                    (Abstract *)Ptr_Wrapped(sm->m, it->stack[i], 0), 
                    NULL
                };
                total += Fmt(sm, "  $: $+$/$ = @\n", args);
            }
            if(i > 0 && it->stack[i] != NULL){
                ptr = *((void **)it->stack[i]);
            }
        }
        word previous = sm->type.state;
        Abstract *args2[] = {
            (Abstract *)((it->p->type.state & FLAG_SPAN_RAW) ?
                I64_Wrapped(sm->m, (i64)it->value) : it->value),
            NULL
        };
        total += Fmt(sm, "value=$>", args2);
    }else if(flags & MORE){
        Abstract *args[] = {
            (Abstract *)StreamTask_Make(sm->m, NULL, (Abstract *)it, ToS_FlagLabels),
            (Abstract *)I32_Wrapped(sm->m, it->idx),
            (Abstract *)I32_Wrapped(sm->m, it->p->nvalues),
            NULL
        };
        total += Fmt(sm, "I<$:$ of $>", args);
    }else{
        total += _ToStream_NotImpl(FUNCNAME, FILENAME, LINENUMBER, sm, a, type, flags);
    }
    return total;
}

status Mem_InitLabels(MemCh *m, Lookup *lk){
    status r = READY;
    if(iterLabels == NULL){
        iterLabels = (Str **)Arr_Make(m, 17);
        iterLabels[9] = Str_CstrRef(m, "GET");
        iterLabels[10] = Str_CstrRef(m, "SET");
        iterLabels[11] = Str_CstrRef(m, "REMOVE");
        iterLabels[12] = Str_CstrRef(m, "RESERVE");
        iterLabels[13] = Str_CstrRef(m, "RESIZE");
        iterLabels[14] = Str_CstrRef(m, "ADD");
        iterLabels[15] = Str_CstrRef(m, "LAST");
        iterLabels[16] = Str_CstrRef(m, "REVERSE");
        Lookup_Add(m, lk, TYPE_ITER, (void *)iterLabels);
        r |= SUCCESS;
    }
    return r;
}

status Mem_ToSInit(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_MEMCTX, (void *)MemCh_Print);
    r |= Lookup_Add(m, lk, TYPE_BOOK, (void *)MemBook_Print);
    r |= Lookup_Add(m, lk, TYPE_MEMSLAB, (void *)MemPage_Print);
    r |= Lookup_Add(m, lk, TYPE_SPAN, (void *)Span_Print);
    r |= Lookup_Add(m, lk, TYPE_ITER, (void *)Iter_Print);
    return r;
}
