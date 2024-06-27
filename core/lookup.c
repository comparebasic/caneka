#include <external.h>
#include <caneka.h>

void *Lookup_Get(Lookup *lk, word type){
    if(type < lk->offset){
        Fatal("Adding lookup value below zero", TYPE_UNIT);
    }
    return (void *)Span_Get(lk->values, (int)(type-lk->offset));
}

status Lookup_Add(MemCtx *m, Lookup *lk, word type, void *value){
    if(type < lk->offset){
        Fatal("Adding lookup value below zero", TYPE_UNIT);
    }
    return Span_Set(m,
        lk->values, (int)(type-lk->offset), (Abstract *)value);
}

Lookup *Lookup_Make(MemCtx *m, word offset, LookupPopulate populate, Abstract *arg){
    Lookup *lk = (Lookup *)MemCtx_Alloc(m, sizeof(Lookup));
    lk->offset = offset;
    lk->values = Span_Make(m);
    lk->arg = arg;
    populate(m, lk);

    return lk;
}
