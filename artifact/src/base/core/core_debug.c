#include <external.h>
#include <caneka.h>


status CoreDebug_Init(MemCtx *m, Lookup *lk){
    status r = READY;
    /*
    r |= Lookup_Add(m, lk, TYPE_ABSTRACT, (void *)Abstract_Print);
    r |= Lookup_Add(m, lk, TYPE_GUARD, (void *)Guard_Print);
    */
    return r;
}
