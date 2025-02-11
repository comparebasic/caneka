#include <external.h>
#include <caneka.h>

void Handler_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    Handler *h = (Handler *)as(a, TYPE_HANDLER);
    if(extended){
        printf("\x1b[%dmH<%d %s", color, h->id, State_ToChars(h->type.state));
    }else{
        printf("\x1b[%dmH<%d %s", color, h->id, State_ToChars(h->type.state));
    }
    if(extended){
        Debug_Print((void *)h->prior, 0, "prior:", color, TRUE);
    }
    printf("\x1b[%dm>\x1b[0m", color);
}
