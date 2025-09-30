#include <external.h>
#include <caneka.h>

PersistItem *PersistItem_Make(MemCh *m, i32 slIdx, void *ptr, cls typeOf){
    PersistItem *item = MemCh_AllocOf(m, sizeof(PersistItem), TYPE_PERSIST_ITEM);
    item->type.of = TYPE_PERSIST_ITEM;
    item->ptr = ptr;
    PersistCoord_Fill(&item->coord, slIdx, ptr, typeOf);
    return item;
}
