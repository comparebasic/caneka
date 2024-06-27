#include <external.h>
#include <caneka.h>

void *Lookup_Get(Lookup *lk, word type){
    return (void *)Span_Get(lk->values, type-lk->offset);
}

status Lookup_Add(Lookup *lk, word type, void *value){
    return Span_Set(m,
        lk->values, TYPE_METHOD_UPDATE-lk->offset,  (Virtual *)value);
}

Lookup *Lookup_Make(MemCtx *m, word offset, LookupPopulate populate, Virtual *arg){
    Lookup *lk = (Lookup *)MemCtx_Alloc(m, sizeof(Lookup));
    lk->offset = offset;
    lk->values = Span_Make(m);
    lk->arg = arg;
    populate(m, lk);

    return lk;
}
