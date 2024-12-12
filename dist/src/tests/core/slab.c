#include <external.h>
#include <caneka.h>

status Slab_setSlot(void *sl, SpanDef *def, int idx, void *value, size_t sz){
    int pos = idx*def->idxSize*sizeof(void *);
    memcpy(sl+pos, value, sz);
    if(def->idxSize > 1){
        QueueIdx *qidx = sl+pos+SPAN_SLOT_BYTES;
        memcpy(sl+SPAN_SLOT_BYTES, sl+pos+SPAN_SLOT_BYTES, (def->idxSize-1)*SPAN_SLOT_BYTES);
    }
    return SUCCESS;
}
void **Slab_nextSlotPtr(void *sl, SpanDef *def, int localIdx){
    int pos = localIdx*(def->idxSize)*sizeof(void *);
    return sl+pos;
}

void *Slab_nextSlot(void *sl, SpanDef *def, int localIdx){
    void **ptr = Slab_nextSlotPtr(sl, def, localIdx); 
    return *ptr;
}

void *Slab_valueAddr(void *sl, SpanDef *def, int localIdx){
    int pos = localIdx*(def->slotSize)*sizeof(void *);
    /*
    printf("Getting Value(%d/%d) from slab:%p at 0x%lx\n", localIdx, pos, sl, (util)(sl+pos));
    */
    return sl+pos;
}
