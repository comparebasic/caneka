#include <external.h>
#include <caneka.h>

void Fatal(char *msg, cls t){
    printf("Fatal Error:");
    printf("%s\n", msg);
    exit(13);
}

void Error(char *msg){
    printf("Error: %s\n", msg);
}
