#include <external.h>
#include "base_module.h"

struct lookup *ExtFreeLookup = NULL;

status MemCh_AddExtFree(MemCh *m, void *a){
    if(m->extFree == NULL){
        m->extFree = Span_Make(m);
    }
    return Span_Add(m->extFree, a);
}

status MemCh_ExtFree(MemCh *m){
    if(m->extFree != NULL){
        status r = READY;
        Iter it;
        Iter_Init(m->extFree);
        word typeOf = ZERO;
        while((Iter_Next(&it) & END) != 0){
            Abstract *a = Iter_Get(&it);
            if(a->type.of != typeOf){
                DoFunc *func = Lookup_Get(ExtFreeLookup, a->type.of);
                if(func == NULL){
                    Error(m, FUNCNAME, FILENAME, LINENUMBER,
                        "Error with extFree func not found", NULL);
                    return ERROR;
                }
                r |= func(m, a);
                if(r & ERROR){
                    break;
                }
            }
        }
        return r;
    }
    return NOOP;
}

status ExtFree_Init(MemCh *m){
    status r = READY;
    if(ExtFreeLookup == NULL){
        ExtFreeLookup = Lookup_Make(m, ZERO);
        r |= SUCCESS;
    }
    return r;
}
