#include <external.h>
#include <filestore.h>

Slab* Slab_Alloc(MemCtx* m, int size, status flags){
    Slab *sl = MemCtx_Alloc(m, sizeof(Slab));
    sl->type.state = flags;
    sl->nallocated = size;

    return sl;
}

