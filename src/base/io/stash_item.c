#include <external.h>
#include <caneka.h>

StashItem *StashItem_Make(MemCh *m, i32 slIdx, void *ptr, cls typeOf){
    StashItem *item = MemCh_AllocOf(m, sizeof(StashItem), TYPE_STASH_ITEM);
    item->type.of = TYPE_STASH_ITEM;
    item->ptr = ptr;
    StashCoord_Fill(&item->coord, slIdx, ptr, typeOf);
    return item;
}
