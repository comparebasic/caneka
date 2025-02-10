#include <external.h>
#include <caneka.h>

void MemCtx_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
   MemCtx *m = (MemCtx*)as(a, TYPE_MEMCTX); 
   MemSlab *sl = m->start_sl;
   printf("\x1b[%dmMemCtx(%p)<", color, m);
       printf("(idx^level)[slabAddr/available]=(");
   while(sl != NULL){
       printf("(%d^%d)[%p/%ld]",sl->idx, sl->level, sl, MemSlab_Available(sl));
       sl = sl->next;
       if(sl != NULL){
         printf(", ");
       }
   }
 printf(")");
   printf("\x1b[%d>\x1b[0m", color);
}
