#include <external.h>
#include <caneka.h>

Lookup *CloneLookup = NULL;

status Clone_Init(MemCh *m){
    status r = READY;
    if(CloneLookup == NULL){
        CloneLookup = Lookup_Make(m, _TYPE_ZERO);
        Lookup *lk = CloneLookup;
        r |= Lookup_Add(m, lk, TYPE_STR, (void *)Str_Clone);
        r |= Lookup_Add(m, lk, TYPE_WRAPPED, (void *)Single_Clone);
        r |= Lookup_Add(m, lk, TYPE_HASHED, (void *)Hashed_Clone);
        r |= Lookup_Add(m, lk, TYPE_SPAN, (void *)Span_Clone);
        r |= Lookup_Add(m, lk, TYPE_STRVEC, (void *)StrVec_Clone);
    }
    if(r == READY){
        r |= NOOP;
    }
    return r;
}

Abstract *Clone(MemCh *m, Abstract *a){
    if(a == NULL){
        return NULL;
    }
    Maker mk = (Maker)Lookup_Get(CloneLookup, Ifc_Get(a->type.of));
    if(mk != NULL){
       return mk(m, a);
    }
    Abstract *args[] = {
        (Abstract *)Type_ToStr(m, a->type.of),
        NULL
    };
    Error(m, FUNCNAME, FILENAME, LINENUMBER, "Unable to clone type $", args);
    return NULL;
}
