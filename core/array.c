#include <external.h>
#include <caneka.h>

int Array_Length(void **arr){
    Abstract **uarr = (Abstract **)arr;
    int i = 0;
    while(uarr[i] != NULL){i++;}
    return i;
}

Array Array_Make(MemCtx *m, int count){
    Array slab = (Abstract **)MemCtx_Alloc(m, sizeof(void*)*(count+1));
    slab[count] = NULL;
    return slab;
}

Array Array_MakeFrom(MemCtx *m, int count, ...){
	va_list args;
    va_start(args, count);
    
    Array slab = (Abstract **)Array_Make(m, count);
    for(int i = 0; i < count; i++){
        slab[i] = va_arg(args, Abstract*);
    }

    return slab;
}
