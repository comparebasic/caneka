#include <external.h>
#include <caneka.h>

Slab* Slab_Alloc(MemCtx* m, status flags, byte slotSize){
    Slab *sl = MemCtx_Alloc(m, sizeof(Slab));
    sl->type.of = TYPE_SLAB;
    sl->type.state = flags;
    sl->slotSize = slotSize;

    return sl;
}

void *Slab_GetPtr(Slab *sl, int idx){
    size_t offset = sizeof(void*)*idx;
    return ((void *)sl->items)+offset;
}
