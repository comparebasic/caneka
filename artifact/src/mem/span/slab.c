#include <external.h>
#include <caneka.h>

status Slab_setSlot(void *sl, int idx, void *value){
    int pos = idx*SLOT_BYTE_SIZE;
    memcpy(sl+pos, value, SLOT_BYTE_SIZE);
    return SUCCESS;
}

void **Slab_nextSlotPtr(void *sl, int localIdx){
    int pos = localIdx*SLOT_BYTE_SIZE;
    return sl+pos;
}

void *Slab_nextSlot(void *sl, int localIdx){
    void **ptr = Slab_nextSlotPtr(sl, localIdx); 
    return *ptr;
}

void *Slab_valueAddr(void *sl, int localIdx){
    int pos = localIdx*SLOT_BYTE_SIZE;
    return sl+pos;
}

void *Slab_Make(MemCtx *m){
    return MemCtx_Alloc(m, SLOT_BYTE_SIZE*SPAN_STRIDE);
}
