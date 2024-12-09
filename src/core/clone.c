#include <external.h>
#include <caneka.h>

Chain *CloneChain = NULL;

static status populateClone(MemCtx *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_STRING, (void *)String_Clone);
    r |= Lookup_Add(m, lk, TYPE_WRAPPED, (void *)Single_Clone);
    r |= Lookup_Add(m, lk, TYPE_HASHED, (void *)Hashed_Clone);
    r |= Lookup_Add(m, lk, TYPE_SPAN, (void *)Span_Clone);
    
    return r;
}

status Clone_Init(MemCtx *m){
    if(CloneChain == NULL){
        Lookup *funcs = Lookup_Make(m, _TYPE_START, populateClone, NULL);
        CloneChain = Chain_Make(m, funcs);
        return SUCCESS;
    }
    return NOOP;
}

Abstract *Clone(MemCtx *m, Abstract *a){
    Maker mk = (Maker)Chain_Get(CloneChain, a->type.of);
    if(mk != NULL){
       return mk(m, a);
    }
    return NULL;
}
