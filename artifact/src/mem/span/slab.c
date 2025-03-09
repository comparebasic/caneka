#include <external.h>
#include <caneka.h>


void **Slab_nextSlotPtr(slot *sl, i32 localIdx){
    return sl+localIdx;
}

void *Slab_nextSlot(slot *sl, i32 localIdx){
    void **ptr = Slab_nextSlotPtr(sl, localIdx); 
    return *ptr;
}
