#include <external.h>
#include <caneka.h>

static void wsOut(i8 dim){
    while(dim-- > 0){
        printf("    ");
    }
}

void SpanState_Print(SpanState *st, i8 dim, int color){
    printf("\x1b[%dmSpanState<%pslab, %hddim, %dlocalIdx, %doffset>\x1b[0m", 
        color, st, dim, st->localIdx, st->offset);
}

void Slab_Print(slab *slab, i8 dim, i8 dims, int color){
    printf("\x1b[%dm(%p){", color, slab);
    void **slot = (void **)slab;
    void **end = slot+SPAN_STRIDE;
    int i = 0;

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
                wsOut(dims-(dim-1));
                printf("\x1b[%dm{\n", color);
                wsOut(dims-(dim-1)+1);
                printf("%d=", i);
                Slab_Print(*slot, dim-1, dims, color); 
            }
            i++;
            slot++;
        }
        printf("\n");
    }

    printf("\x1b[%dm}\x1b[0m", color);
}

void Span_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    Span *p = (Span*)as(a, TYPE_SPAN); 
    printf("\x1b[%dm%sSpan<\x1b[1;%dm%d\x1b[0;%dmval/\x1b[1;%dm%hd\x1b[0;%dmdim ", color, msg, color, p->nvalues, color, color, p->dims, color);
    Slab_Print(p->root, p->dims, p->dims, color);
    printf("\x1b[%dm>\x1b[0m", color);
}

void MemSlab_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    MemSlab *sl = (MemSlab*)as(a, TYPE_MEMSLAB); 
    int bcolor = 0;
    int lcolor = color;
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
}

void MemCtx_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    MemCtx *m = (MemCtx*)as(a, TYPE_MEMCTX); 
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
             MemSlab_Print((Abstract *)sl, 0, "", color, extended);
             if((it.type.state & FLAG_ITER_LAST) == 0){
                printf(", ");
             }
         }
    }

    printf(")");
    printf("\x1b[%d>\x1b[0m", color);
}

void MemChapter_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    MemChapter *cp = (MemChapter*)as(a, TYPE_CHAPTER); 
    printf("\x1b[%dm%sMemChapter<start:%p/pages:%d)[available:%d selected:%d]",
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
             MemSlab_Print((Abstract *)sl, 0, "", color, extended);
             if((it.type.state & FLAG_ITER_LAST) == 0){
                printf(", ");
             }
         }
    }
    printf("\x1b[%d>\x1b[0m", color);
}

status MemDebug_Init(MemCtx *m, Lookup *lk){
    status r = READY;
    /*
    r |= Lookup_Add(m, lk, TYPE_MEMCTX, (void *)MemCtx_Print);
    r |= Lookup_Add(m, lk, TYPE_CHAPTER, (void *)MemChapter_Print);
    r |= Lookup_Add(m, lk, TYPE_MEMSLAB, (void *)MemSlab_Print);
    */
    return r;
}
