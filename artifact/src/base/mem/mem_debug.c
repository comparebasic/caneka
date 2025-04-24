#include <external.h>
#include <caneka.h>

static inline i64 wsOut(Stream *sm, i8 dim){
    while(dim-- > 0){
        return Stream_To(sm, (byte *)"    ", 4); 
    }
    return 0;
}


i64 MemPage_Print(Stream *sm, Abstract *a, cls type, boolean extended){
    MemPage *sl = (MemPage*)as(a, TYPE_MEMSLAB); 
    /*
    i32 bcolor = 0;
    i32 lcolor = color;
    if(sl->level > 0){
        bcolor = color+10; 
        lcolor = 30;
    }
    if(sl->bytes == NULL){
        printf("(\x1b[1;%d;%dmnullBytes \x1b[0;%dm ", color+10, color, color);
    }else{
        printf("(");
    }
    printf("%p/%p:\x1b[%d;%dm%d\x1b[0;%dm)[\x1b[1;%dm%hd\x1b[0;%dm/%hd]", 
         sl, sl->bytes, bcolor, lcolor, sl->level, color, color, 
         MemPage_Taken(sl), color, MemPage_Available(sl));
     */
    return 0;
}

i64 MemCh_Print(Stream *sm, Abstract *a, cls type, boolean extended){
    MemCh *m = (MemCh*)as(a, TYPE_MEMCTX); 
    /*
    printf("\x1b[%dm%sMemCh(%p)<%d ", color, msg, m, m->p.nvalues);
    printf("(slabAddr:idx^level)[used/available]=(");

    Iter it;
    Iter_Init(&it, &m->p);
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

    printf(")");
    printf("\x1b[%d>\x1b[0m", color);
    */
    return 0;
}

i64 MemBook_Print(Stream *sm, Abstract *a, cls type, boolean extended){
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

i64 Span_Print(struct stream *sm, Abstract *a, cls type, boolean extended){
    Span *p = (Span*)as(a, TYPE_SPAN); 

    i64 total = 0;
    Abstract *args[] = {
        (Abstract *)I32_Wrapped(sm->m, p->nvalues), 
        (Abstract *)I32_Wrapped(sm->m, p->max_idx), 
        (Abstract *)I8_Wrapped(sm->m, p->dims),
        NULL
    };
    total += Fmt(sm, "Span<^D.$^d.values/0..$/$dims [", args);
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
            total += Fmt(sm, "$:$", args);
            if((it.type.state & FLAG_ITER_LAST) == 0){
                total += Stream_To(sm, (byte *)", ", 2);
            }
        }
    }
    total += Stream_To(sm, (byte *)"]>", 2);
    
    return total;
}

i64 Iter_Print(Stream *sm, Abstract *a, cls type, boolean extended){
    Iter *it = (Iter *)as(a, TYPE_ITER);
    Str *s = State_ToStr(sm->m, it->type.state);
    if(extended){
        i64 total = 0;
        Abstract *args[] = {
            (Abstract *)s,
            (Abstract *)I32_Wrapped(sm->m, it->idx),
            (Abstract *)I32_Wrapped(sm->m, it->span->nvalues),
            (Abstract *)I8_Wrapped(sm->m, it->span->dims),
            NULL
        };
        total += Fmt(sm, "I<$@$ of $/$dims\n", args);
        void *ptr = it->span->root;
        for(i8 i = it->span->dims; i >= 0; i--){
            if(it->stack[i] == NULL){
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
                    (Abstract *)Ptr_Wrapped(sm->m, ptr),
                    (Abstract *)I32_Wrapped(sm->m, delta),
                    (Abstract *)I32_Wrapped(sm->m, it->stackIdx[i]),
                    (Abstract *)Ptr_Wrapped(sm->m, it->stack[i]), 
                    NULL
                };
                total += Fmt(sm, "  $: $+$/$ = $\n", args);
            }
            if(i > 0 && it->stack[i] != NULL){
                ptr = *((void **)it->stack[i]);
            }
        }
        word previous = sm->type.state;
        sm->type.state |= DEBUG;
        Abstract *args2[] = {
            (Abstract *)it->value,
            NULL
        };
        total += Fmt(sm, "value=$>", args2);
        sm->type.state = previous;
        return total;
    }else{
        Abstract *args[] = {
            (Abstract *)s,
            (Abstract *)I32_Wrapped(sm->m, it->idx),
            (Abstract *)I32_Wrapped(sm->m, it->span->nvalues),
            NULL
        };
        return Fmt(sm, "I<$:$ of $>", args);
    }
}

status Mem_DebugInit(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_MEMCTX, (void *)MemCh_Print);
    r |= Lookup_Add(m, lk, TYPE_BOOK, (void *)MemBook_Print);
    r |= Lookup_Add(m, lk, TYPE_MEMSLAB, (void *)MemPage_Print);
    r |= Lookup_Add(m, lk, TYPE_SPAN, (void *)Span_Print);
    r |= Lookup_Add(m, lk, TYPE_ITER, (void *)Iter_Print);
    return r;
}
