#include <external.h>

#include "../../base/types/ifc.c"

word GLOBAL_flags = 0;
word NORMAL_FLAGS = 0b0000000011111111;
word UPPER_FLAGS = 0b1111111100000000;

void *Fatal(i32 fd, char *func, char *file, int line, char *fmt, ...){
    printf("poo: %s\n", fmt);
    return NULL;
}

void *MemBook_GetBytes(){
    return NULL;
}

void *MemSlab_Alloc(MemSlab *sl, word sz){
    return NULL;
}


#define MemCh_Alloc(m, sz) malloc(sz)
