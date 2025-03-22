#include <external.h>
#include <caneka.h>

status Guard_Init(Guard *g, int max){
    memset(g, 0, sizeof(Guard));
    g->type.of = TYPE_GUARD;
    g->max = max;
    return SUCCESS;
}

status Guard_Setup(MemCtx *m, Guard *g, int max, byte *msg){
    return Guard_Init(g, max);
}

status Guard_Reset(Guard *g){
    g->count = 0;
    return SUCCESS;
}

status _Guard_Incr(Guard *g, char *func, char *file, int line){
    if(++g->count > g->max){
        g->type.state |= ERROR;
        g->file = file;
        g->line = line;
        _Fatal("Guard failure", TYPE_GUARD, 0, func, file, line);
        return g->type.state;
    }
    return NOOP;
}

