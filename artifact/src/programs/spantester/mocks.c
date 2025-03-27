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

i64 Iter_Print(MemCh *m, StrVec *v, i32 fd, Abstract *a, cls type, boolean extended){
    Iter *it = (Iter *)a;
    printf("It<%d root:*%lu/\x1b[1m%lu\x1b[22m\n", it->idx, (util)it->span->root, (util)*((void **)it->span->root));
    i8 dim = it->span->dims;
    while(dim >= 0){
        if(it->stack[dim] > 0){
            util delta = 0;
            if(dim == it->span->dims || it->stack[dim+1] != NULL){
                util base = 0;
                if(dim == it->span->dims){
                    base = (util)it->span->root; 
                }else{
                    base = (util)it->stack[dim+1];
                }
                delta = ((util)it->stack[dim] - (util)base)/8;
            }
            printf("   dim:%d localIdx:%d = (+%lu) *%lu/\x1b[1m%lu\x1b[22m\n", 
                (i32)dim, it->stackIdx[dim], delta, (util)it->stack[dim], 
                it->stack[dim] != NULL ? (util)*((void **)it->stack[dim]) : 0);
        }else{
            printf("   dim:%d localIdx:%d = *%lu/\x1b[1m%lu\x1b[22m\n", 
                (i32)dim, it->stackIdx[dim], (util)it->stack[dim], 
                it->stack[dim] != NULL ? (util)*((void **)it->stack[dim]) : 0);
        }
        dim--;
    }
    printf(">\n");
    return 0;
}

