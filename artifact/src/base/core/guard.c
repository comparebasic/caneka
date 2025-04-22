#include <external.h>
#include <caneka.h>

status Guard_Reset(i16 *g){
    *g = 0;
    return SUCCESS;
}

status Guard_Incr(i16 *g, i16 max, char *func, char *file, int line){
    if(++(*g) > max){
        Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "Guard failure", NULL);
        return ERROR;
    }
    return NOOP;
}
