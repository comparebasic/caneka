#include <external.h>
#include <caneka.h>

void MemSlab_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    MemSlab *sl = (MemSlab*)as(a, TYPE_MEMSLAB); 
    int bcolor = 0;
    int lcolor = color;
    if(sl->level > 0){
        bcolor = color+10; 
        lcolor = 30;
    }
    if(sl->bytes == NULL){
        printf("(\x1b[1;%d;%dmnullBytes\x1b[0;%dm ", color+10, color, color);
    }else{
        printf("(");
    }
    printf("%p:\x1b[%d;%dm%d\x1b[0;%dm)[\x1b[1;%dm%hd\x1b[0;%dm/%hd]", 
         sl, bcolor, lcolor, sl->level, color, color, MemSlab_Taken(sl), color,
         MemSlab_Available(sl));
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
