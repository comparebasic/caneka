#include <external.h>
#include <caneka.h>

void SlabResult_SetStack(SlabResult *sr, void *ptr, byte dim){
    if(dim > MAX_DIMS){
        Fatal("Greater dim than available stack", TYPE_SPAN);
    }else{
        sr->stack[dim] = ptr;
    }
}


status Slab_setSlot(void *sl, SpanDef *def, int idx, void *value, size_t sz){
    int pos = idx*def->idxSize*sizeof(void *);
    memcpy(sl+pos, value, sz);
    if(def->idxSize > 1){
        QueueIdx *qidx = sl+pos+SPAN_SLOT_BYTES;
        printf("copying extras %s\n", QueueFlags_ToChars( qidx->flags));
        memcpy(sl+SPAN_SLOT_BYTES, sl+pos+SPAN_SLOT_BYTES, (def->idxSize-1)*SPAN_SLOT_BYTES);
    }
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
