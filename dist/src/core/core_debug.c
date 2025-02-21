#include <external.h>
#include <caneka.h>

static Abstract *Print(MemCtx *m, Abstract *a){
    Debug_Print(a, 0, "", 0, TRUE);
    return NULL;
}

static void Guard_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
   Guard *g = (Guard *)as(a, TYPE_GUARD);
   if((g->type.state & ERROR) != 0){
        color = COLOR_RED;
   }
   printf("\x1b[%dm%sG<count %d of max %d ", color, msg, g->count, g->max);
   Debug_Print((void *)&g->msg, 0, "", color, FALSE);
   printf(" \x1b[%dm%s:%d>\x1b[0m", color, g->file, g->line);
}

status CoreDebug_Init(MemCtx *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_ABSTRACT, (void *)Abstract_Print);
    r |= Lookup_Add(m, lk, TYPE_GUARD, (void *)Guard_Print);
    return r;
}
