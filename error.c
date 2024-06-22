#include "external.h"
#include "filestore.h"

void Fatal(char *msg, type t){
    printf("Fatal Error:");
    printf("%s\n", msg);
    exit(13);
}

void Error(char *msg){
    printf("Error: %s\n", msg);
}
