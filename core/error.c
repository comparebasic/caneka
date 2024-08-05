#include <external.h>
#include <caneka.h>

void *_Fatal(char *msg, cls t, char *func, char *file, int line){
    printf("Fatal Error: %s - type(%s) %s:%s:%d\n" , msg, Class_ToString(t), func, file, line);
    exit(13);
    return NULL;
}

void *Error(char *msg){
    printf("Error: %s\n", msg);
    return NULL;
}
