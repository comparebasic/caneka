#include <external.h>
#include <caneka.h>

#include "../inc/handle_io.c"

static inline i64 wsOut(MemCtx *m, StrVec *v, i32 fd, i8 dim){
    while(dim-- > 0){
        return StrVec_Add(v, Str_Ref(m, (byte *)"    ", 4, 4)); 
    }
    return 0;
}

i64 SpanState_Print(MemCtx *m, StrVec *v, i32 fd, Abstract *a, cls type, boolean extended){
    SpanState *st = (SpanState *)a;
    i32 idx = st->localIdx;
    void *slot = NULL;
    if(st->slab != NULL){
        slot = *(((void **)st->slab) + st->localIdx);
    }
    return StrVec_FmtAdd(m, v, fd, "SpanState<^B_i4^b^o+idx, ^B_a^b^slot, ^B_a^b^slab, ^B_i4^b^localIdx, ^B_i4^b^offset>", 
        idx, slot, st->slab, (i32)st->localIdx, (i32)st->offset);
}

i64 Slab_Print(MemCtx *m, StrVec *v, i32 fd, slab *slab, i8 dim, i8 dims){
    /*
    printf("\x1b[%dm(%p){", color, slab);
    void **slot = (void **)slab;
    void **end = slot+SPAN_STRIDE;
    i32 i = 0;

    while(slot < end){
        if(*slot != NULL){
            if(slot != (void **)slab){
                printf(", ");
            }
            printf("\x1b[1;%dm%d\x1b[0;%dm=%p", color, i, color, *slot);
        }
        i++;
        slot++;
    }
    printf("}\x1b[0m");

    if(dim > 0){
        slot = (void **)slab;
        end = slot+SPAN_STRIDE;
        i = 0;
        while(slot < end){
            if(*slot != NULL){
                printf("\n");
                wsOut(m, v, dims-(dim-1));
                printf("\x1b[%dm{\n", color);
                wsOut(m, v, dims-(dim-1)+1);
                printf("%d=", i);
                Slab_Print(m, v, *slot, dim-1, dims, color); 
            }
            i++;
            slot++;
        }
        printf("\n");
    }

    printf("\x1b[%dm}\x1b[0m", color);
    */
    return 0;
}

i64 Span_Print(MemCtx *m, StrVec *v, i32 fd, Abstract *a, cls type, boolean extended){
    Span *p = (Span*)as(a, TYPE_SPAN); 
    /*
    printf("\x1b[%dm%sSpan<\x1b[1;%dm%d\x1b[0;%dmval/\x1b[1;%dm%hd\x1b[0;%dmdim ", color, msg, color, p->nvalues, color, color, p->dims, color);
    Slab_Print(m, v, p->root, p->dims, p->dims, color);
    printf("\x1b[%dm>\x1b[0m", color);
    */
    return 0;
}

i64 MemSlab_Print(MemCtx *m, StrVec *v, i32 fd, Abstract *a, cls type, boolean extended){
    MemSlab *sl = (MemSlab*)as(a, TYPE_MEMSLAB); 
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
         MemSlab_Taken(sl), color, MemSlab_Available(sl));
     */
    return 0;
}

i64 MemCtx_Print(MemCtx *_m, StrVec *v, i32 fd, Abstract *a, cls type, boolean extended){
    MemCtx *m = (MemCtx*)as(a, TYPE_MEMCTX); 
    /*
    printf("\x1b[%dm%sMemCtx(%p)<%d ", color, msg, m, m->p.nvalues);
    printf("(slabAddr:idx^level)[used/available]=(");

    Iter it;
    Iter_Init(&it, &m->p);
    while((Iter_Next(&it) & END) == 0){
         MemSlab *sl = (MemSlab *)Iter_Get(&it);
         if(sl != NULL){
             if(extended){
                printf("\n    ");
             }
             printf("\x1b[%dm%d:", color, it.idx);
             MemSlab_Print(m, v, (Abstract *)sl, 0, color, extended, "");
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

i64 MemBook_Print(MemCtx *m, StrVec *v, i32 fd, Abstract *a, cls type, boolean extended){
    MemBook *cp = (MemBook*)as(a, TYPE_BOOK); 
/*
    printf("\x1b[%dm%sMemBook<start:%p/pages:%d)[available:%d selected:%d]",
        color, msg, cp->start, cp->pages.nvalues, cp->pages.metrics.available,
        cp->pages.metrics.selected);
    Iter it;
    Iter_Init(&it, &cp->pages);
    while((Iter_Next(&it) & END) == 0){
         MemSlab *sl = (MemSlab *)Iter_Get(&it);
         if(sl != NULL){
             if(extended){
                printf("\n    ");
             }
             printf("\x1b[%dm%d:", color, it.idx);
             MemSlab_Print(m, v, (Abstract *)sl, 0, color, extended, "");
             if((it.type.state & FLAG_ITER_LAST) == 0){
                printf(", ");
             }
         }
    }
    printf("\x1b[%d>\x1b[0m", color);
    */
    return 0;
}

status Mem_DebugInit(MemCtx *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_MEMCTX, (void *)MemCtx_Print);
    r |= Lookup_Add(m, lk, TYPE_BOOK, (void *)MemBook_Print);
    r |= Lookup_Add(m, lk, TYPE_MEMSLAB, (void *)MemSlab_Print);
    r |= Lookup_Add(m, lk, TYPE_SPAN, (void *)Span_Print);
    r |= Lookup_Add(m, lk, TYPE_SPANSTATE, (void *)SpanState_Print);
    return r;
}
