#include <external.h>
#include <caneka.h>

void PersistCoord_Fill(PersistCoord *coord, i32 slIdx, void *ptr, cls typeOf){
    coord->typeOf = typeOf;
    coord->idx = slIdx;
    coord->offset = (quad)(((util)ptr) & MEM_PERSIST_MASK);
}
