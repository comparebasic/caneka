#include <external.h>
#include <caneka.h>

Slab* Slab_Alloc(MemCtx* m, status flags){
    Slab *sl = MemCtx_Alloc(m, sizeof(Slab));
    sl->type.of = TYPE_SLAB;
    sl->type.state = flags;

    return sl;
}
