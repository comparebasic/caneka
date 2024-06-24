#include <external.h>
#include <filestore.h>

Slab* Slab_Alloc(MemCtx* m, status flags){
    Slab *sl = MemCtx_Alloc(m, sizeof(Slab));
    sl->type.state = flags;

    return sl;
}
