#include <external.h>
#include <caneka.h>

status Guard_Init(Guard *g, int max){
    memset(g, 0, sizeof(Guard));
    g->type.of = TYPE_GUARD;
    g->max = max;
    g->msg.type.of = TYPE_STRING_CHAIN;
    return SUCCESS;
}

status Guard_Setup(MemCtx *m, Guard *g, int max, byte *msg){
    Guard_Init(g, max);
    return String_AddBytes(m, &g->msg, msg, strlen((char *)msg));
}

status Guard_Reset(Guard *g){
    g->count = 0;
    return String_Reset(&g->msg);
}

status _Guard_Incr(Guard *g, char *func, char *file, int line){
    if(++g->count > g->max){
        g->type.state |= ERROR;
        g->file = file;
        g->line = line;
        Debug_Print((void *)g, 0, "", 0, TRUE);
        printf("\n");
        _Fatal("Guard failure", TYPE_GUARD, func, file, line);
        return g->type.state;
    }
    return NOOP;
}

