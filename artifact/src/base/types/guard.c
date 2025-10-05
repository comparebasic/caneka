#include <external.h>
#include <caneka.h>

status Guard_Reset(i16 *g){
    *g = 0;
    return SUCCESS;
}

boolean Guard(i16 *g, i16 max, char *func, char *file, int line){
    return ++(*g) <= max;
}

status Guard_Incr(i16 *g, i16 max, char *func, char *file, int line){
    if(Guard(g, max, func, file, line)){
        return SUCCESS;
    }
    Single sg = {.type = {TYPE_WRAPPED_I16, 0}, .val.w = *g};
    Single max_sg = {.type = {TYPE_WRAPPED_I16, 0}, .val.w = max};
    Abstract *args[] = {
        (Abstract *)&sg,
        (Abstract *)&max_sg,
        NULL
    };
    Error(ErrStream->m, NULL, func, file, line, "Guard Error $/$", args);
    return ERROR;
}
