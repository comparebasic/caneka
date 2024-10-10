#include <external.h>
#include <caneka.h>

status Slab_setSlot(void *sl, SpanDef *def, int idx, void *value, size_t sz){
    int pos = idx*def->idxSize*sizeof(void *);
    /*
    printf("Setting Idx(%d/%d) of %p to 0x%lx at 0x%lx\n", idx, pos, sl, *((util *)value), (util)(sl+pos));
    */
    memcpy(sl+pos, value, sz);
    return SUCCESS;
}
void **Slab_nextSlotPtr(SlabResult *sr, SpanDef *def, int local_idx){
    void *sl = (void *)sr->slab;
    int pos = local_idx*(def->idxSize)*sizeof(void *);
    return sl+pos;
}

void *Slab_nextSlot(SlabResult *sr, SpanDef *def, int local_idx){
    void **ptr = Slab_nextSlotPtr(sr, def, local_idx); 
    return *ptr;
}

void *Slab_valueAddr(SlabResult *sr, SpanDef *def, int local_idx){
    void *sl = (void *)sr->slab;
    int pos = local_idx*(def->slotSize)*sizeof(void *);
    /*
    printf("Getting Value(%d/%d) from slab:%p at 0x%lx\n", local_idx, pos, sl, (util)(sl+pos));
    */
    return sl+pos;
}
