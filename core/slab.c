#include <external.h>
#include <caneka.h>

status Slab_setSlot(void *sl, SpanDef *def, int idx, void *value, size_t sz){
    int pos = idx*def->slotSize*sizeof(void *);
    printf("\x1b[33mSlab: setting %ld bytes at pos:%p to %p\x1b[0m\n", sz, sl+pos, value);
    memcpy(sl+pos, value, sz);
    return SUCCESS;
}
