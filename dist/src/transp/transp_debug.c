#include <external.h>
#include <caneka.h>

static void Transp_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    Transp *p = (Transp*)as(a, TYPE_TRANSP);
    printf("\x1b[%dm%sTransp<", color, msg);
    if(p->source != NULL){
        Debug_Print((void *)p->source->path, 0, "", color, FALSE);
    }
    Target *t = Span_GetSelected(p->targets->values);
    if(t != NULL){
        Debug_Print((void *)t->path, 0, " -> ", color, FALSE);
    }
    printf("\x1b[%dm>\x1b[0m\n", color);
}
