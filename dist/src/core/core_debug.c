#include <external.h>
#include <caneka.h>

static void Guard_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
   Guard *g = (Guard *)as(a, TYPE_GUARD);
   if((g->type.state & ERROR) != 0){
        color = COLOR_RED;
   }
   printf("\x1b[%dm%sG<count %d of max %d ", color, msg, g->count, g->max);
   Debug_Print((void *)&g->msg, 0, "", color, FALSE);
   printf(" \x1b[%dm%s:%d>\x1b[0m", color, g->file, g->line);
}
