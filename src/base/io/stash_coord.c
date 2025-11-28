#include <external.h>
#include <caneka.h>
#include "../module.h"

void StashCoord_Fill(StashCoord *coord, i32 slIdx, void *ptr, cls typeOf){
    coord->typeOf = typeOf;
    coord->idx = slIdx;
    coord->offset = (quad)(((util)ptr) & MEM_STASH_MASK);
}
