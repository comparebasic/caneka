#include <external.h>
#include <caneka.h>

status Slab_setSlot(void *sl, int idx, byte slotSize, void *value, size_t sz){
    int pos = idx*slotSize*sizeof(void *);
    memcpy(sl+pos, value, sz);
    return SUCCESS;
}
void **Slab_nextSlotPtr(void *sl, byte slotSize, byte ptrSlot, int localIdx){
    int pos = localIdx*(slotSize)*sizeof(void *)+ptrSlot*sizeof(void *);
    return sl+pos;
}

void *Slab_nextSlot(void *sl, byte slotSize, byte ptrSlot, int localIdx){
    void **ptr = Slab_nextSlotPtr(sl, slotSize, ptrSlot, localIdx); 
    return *ptr;
}
