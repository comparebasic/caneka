#include <external.h>
#include <caneka.h>

static inline i64 wsOut(Stream *sm, i8 dim){
    while(dim-- > 0){
        return Stream_Bytes(sm, (byte *)"    ", 4); 
    }
    return 0;
}

i64 MemPage_Print(Stream *sm, Abstract *a, cls type, word flags){
    MemPage *sl = (MemPage*)as(a, TYPE_MEMSLAB); 
    Abstract *args[] = {
        (Abstract *)I16_Wrapped(sm->m, sl->level),
        (Abstract *)I16_Wrapped(sm->m, sl->remaining),
        NULL
    };
   
    return Fmt(sm, "Page<^D.$^d.level ^D.$^d.remaining>", args);
}

i64 MemCh_Print(Stream *sm, Abstract *a, cls type, word flags){
    MemCh *m = (MemCh*)as(a, TYPE_MEMCTX); 
    i64 total = 0;
    Abstract *args[] = {
        (Abstract *)MemCount_Wrapped(sm->m,  MemCh_Used(m, 0)),
        NULL
    };

    total +=  Fmt(sm, "MemCh<used:$ [", args);
    Iter it;
    Iter_Init(&it, m->it.p);
    while((Iter_Next(&it) & END) == 0){
        ToS(sm, it.value, 0, flags);
        if((it.type.state & FLAG_ITER_LAST) == 0){
            Stream_Bytes(sm, (byte *)",", 1);
        }
    }

    total +=  Fmt(sm, "]>", args);
    return total;
}

i64 MemBook_Print(Stream *sm, Abstract *a, cls type, word flags){
    MemBook *cp = (MemBook*)as(a, TYPE_BOOK); 
/*
    printf("\x1b[%dm%sMemBook<start:%p/pages:%d)[available:%d selected:%d]",
        color, msg, cp->start, cp->pages.nvalues, cp->pages.metrics.available,
        cp->pages.metrics.selected);
    Iter it;
    Iter_Init(&it, &cp->pages);
    while((Iter_Next(&it) & END) == 0){
         MemPage *sl = (MemPage *)Iter_Get(&it);
         if(sl != NULL){
             if(extended){
                printf("\n    ");
             }
             printf("\x1b[%dm%d:", color, it.idx);
             MemPage_Print(m, v, (Abstract *)sl, 0, color, extended, "");
             if((it.type.state & FLAG_ITER_LAST) == 0){
                printf(", ");
             }
         }
    }
    printf("\x1b[%d>\x1b[0m", color);
    */
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
                it.value,
                NULL
            };
            if(flags & DEBUG){
                total += Stream_Bytes(sm, (byte *)"\n    ", 5);
            }
            if(flags & (MORE|DEBUG)){
                total += Fmt(sm, "$:@", args);
            }else{
                total += ToS(sm, it.value, 0, flags);
            }
            if((it.type.state & FLAG_ITER_LAST) == 0 && (flags & MORE)){
                total += Stream_Bytes(sm, (byte *)", ", 2);
            }else if (flags & DEBUG){
                total += Stream_Bytes(sm, (byte *)"\n  ", 3);
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
        Str *s = State_ToStr(sm->m, it->type.state);
        Abstract *args[] = {
            (Abstract *)s,
            (Abstract *)I32_Wrapped(sm->m, it->idx),
            (Abstract *)I32_Wrapped(sm->m, it->p->nvalues),
            (Abstract *)I8_Wrapped(sm->m, it->p->dims),
            NULL
        };
        total += Fmt(sm, "I<$@$ of $/$dims\n", args);
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
            (Abstract *)it->value,
            NULL
        };
        total += Fmt(sm, "value=$>", args2);
    }else if(flags & MORE){
        Abstract *args[] = {
            (Abstract *)State_ToStr(sm->m, it->type.state),
            (Abstract *)I32_Wrapped(sm->m, it->idx),
            (Abstract *)I32_Wrapped(sm->m, it->p->nvalues),
            NULL
        };
        total += Fmt(sm, "I<$:$ of $>", args);
    }else{
        total += ToStream_NotImpl(sm, a, type, flags);
    }
    return total;
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
