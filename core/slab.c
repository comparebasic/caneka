#include <external.h>
#include <caneka.h>

status Slab_setSlot(void *sl, SpanDef *def, int idx, void *value, size_t sz){
    int pos = idx*def->slotSize*sizeof(void *);
    printf("Value 0x%lx\n", *((util *)value));
    memcpy(sl+pos, value, sz);
    return SUCCESS;
}
