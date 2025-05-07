#include <external.h>
#include <caneka.h>

Lookup *CloneLookup = NULL;

static status populateClone(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_STR, (void *)Str_Clone);
    r |= Lookup_Add(m, lk, TYPE_WRAPPED, (void *)Single_Clone);
    r |= Lookup_Add(m, lk, TYPE_HASHED, (void *)Hashed_Clone);
    r |= Lookup_Add(m, lk, TYPE_SPAN, (void *)Span_Clone);

    /*
    r |= Lookup_Add(m, lk, TYPE_FILE, (void *)File_Clone);
    */
    return r;
}

status Clone_Init(MemCh *m){
    if(CloneLookup == NULL){
        Lookup *CloneLookup = Lookup_Make(m, _TYPE_ZERO, populateClone, NULL);
        return SUCCESS;
    }
    return NOOP;
}

Abstract *Clone(MemCh *m, Abstract *a){
    if(a == NULL){
        return NULL;
    }
    Maker mk = (Maker)Lookup_Get(CloneLookup, a->type.of);
    if(mk != NULL){
       return mk(m, a);
    }
    return NULL;
}
