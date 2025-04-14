#include <external.h>
#include <caneka.h>

status Guard_Init(Guard *g, int max){
    memset(g, 0, sizeof(Guard));
    g->type.of = TYPE_GUARD;
    g->max = max;
    return SUCCESS;
}

status Guard_Setup(MemCh *m, Guard *g, i32 max, byte *msg){
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
        Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "Guard failure");
        return g->type.state;
    }
    return NOOP;
}

