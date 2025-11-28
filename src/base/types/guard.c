#include <external.h>
#include <caneka.h>
#include "../module.h"

status Guard_Reset(i16 *g){
    *g = 0;
    return SUCCESS;
}

boolean Guard(i16 *g, i16 max, char *func, char *file, int line){
    if(max <= 0){
        return TRUE;
    }

    return ++(*g) <= max;
}

status Guard_Incr(MemCh *m, i16 *g, i16 max, char *func, char *file, int line){
    if(Guard(g, max, func, file, line)){
        return SUCCESS;
    }
    Single sg = {.type = {TYPE_WRAPPED_I16, 0}, .val.w = *g};
    Single max_sg = {.type = {TYPE_WRAPPED_I16, 0}, .val.w = max};
    void *args[] = {
        &sg,
        &max_sg,
        NULL
    };
    Error(m, func, file, line, "Guard Error $/$", args);
    return ERROR;
}
