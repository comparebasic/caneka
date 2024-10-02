#include <external.h>
#include <caneka.h>

status Log(status level, char *msg, ...){
	va_list args;
    va_start(args, msg);
    if(level == ERROR){
        fprintf(stderr, "ERROR: ");
        vprintf(msg, args);
        fprintf(stderr, "\n");
        return SUCCESS;
    }else{
        vfprintf(stderr, msg, args);
        fprintf(stderr, "\n");
        return SUCCESS;
    }
}
