#include <external.h>
#include <filestore.h>

Lookup *Lookup_Make(MemCtx *m, word offset, LookupPopulate populate, Virtual *arg){
    Lookup *lk = (Lookup *)MemCtx_Alloc(m, sizeof(Lookup));
    lk->offset = offset;
    lk->values = Span_Make(m);
    lk->arg = arg;
    populate(m, lk);

    return lk;
}
