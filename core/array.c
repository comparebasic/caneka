#include <external.h>
#include <filestore.h>

int Array_Length(void **arr){
    Unit **uarr = (Unit **)arr;
    int i = 0;
    while(uarr[i] != NULL){i++;}
    return i;
}

void **Array_Make(MemCtx *m, int count){
    Unit **slab = (Unit **)MemCtx_Alloc(m, sizeof(void*)*(count+1));
    slab[count] = NULL;
    return (void **)slab;
}

void *Array_MakeFrom(MemCtx *m, int count, ...){
	va_list args;
    va_start(args, count);
    
    Unit **slab = (Unit **)Array_Make(m, count);
    for(int i = 0; i < count; i++){
        slab[i] = va_arg(args, Unit*);
    }

    return slab;
}
