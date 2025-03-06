#include <external.h>
#include <caneka.h>

void MemCtx_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    MemCtx *m = (MemCtx*)as(a, TYPE_MEMCTX); 
    printf("\x1b[%dmMemCtx(%p)<", color, m);
    printf("(idx^level)[slabAddr/available]=(");

    Iter it;
    Iter_Init(&it, m);
    while((Iter_Next(&it) & END) == 0){
         MemSlab *sl = (MemSlab *)Iter_Get(&it);
         printf("(%d^%d)[%p/%ud]", it.idx, sl->level, sl, MemSlab_Available(sl));
         if((it.type.state & FLAG_ITER_LAST) != 0){
            printf(", ");
         }
    }
    printf(")");
    printf("\x1b[%d>\x1b[0m", color);
}

status MemCtxDebug_Init(MemCtx *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_MEMCTX, (void *)MemCtx_Print);
    return r;
}
