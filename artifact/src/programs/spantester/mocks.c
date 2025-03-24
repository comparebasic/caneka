#include <external.h>

#include "../../base/types/ifc.c"

void *_Fatal(char *msg, cls t, i32 fd, char *func, char *file, int line){
    printf("poo: %s\n", msg);
    return NULL;
}

void *MemBook_GetBytes(){
    return NULL;
}

void *MemSlab_Alloc(MemSlab *sl, word sz){
    return NULL;
}


#define MemCtx_Alloc(m, sz) malloc(sz)
