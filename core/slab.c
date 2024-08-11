#include <external.h>
#include <caneka.h>

Slab* Slab_Alloc(MemCtx* m){
    return MemCtx_Alloc(m, sizeof(Abstract *)*SPAN_DIM_SIZE);
}

void *Slab_GetPtr(Slab *sl, int idx){
    size_t offset = sizeof(void*)*idx;
    return ((void *)sl)+offset;
}
