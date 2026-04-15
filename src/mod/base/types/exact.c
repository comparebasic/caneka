#include <external.h>
#include "base_module.h"

Lookup *ExactLookup = NULL;

status Exact_Init(MemCh *m){
    status r = READY;
    if(ExactLookup == NULL){
        ExactLookup = Lookup_Make(m, _TYPE_ZERO);
        r |= SUCCESS;
    }
    return r;
}

boolean Exact(void *_a, void *_b){
    Abstract *a = (Abstract *)_a;
    Abstract *b = (Abstract *)_b;
    void *args[] = {
        a, b, NULL
    };
    if(a == b){
        goto exact;
    }else if(a == NULL || b == NULL){
        goto nonexact;
    }else if(a->type.of != b->type.of){
        goto nonexact;
    }else if(a->type.of == TYPE_STR){
        if(Str_ExactStr((Str *)a, (Str *)b)){
            goto exact;
        }else{
            goto nonexact;
        }
    }else if(a->type.of == TYPE_STRVEC){
        if(StrVec_ExactStrVec((StrVec *)a, (StrVec *)b)){
            goto exact;
        }else{
            goto nonexact;
        }
    }else if(a->type.of == TYPE_SPAN){
        if(Span_Exact((Span *)a, (Span *)b)){
            goto exact;
        }else{
            goto nonexact;
        }
    }else if(a->type.of == TYPE_WRAPPED_PTR){
        if(Single_Exact((Single *)a, (Single *)b)){
            goto exact;
        }else{
            goto nonexact;
        }
    }
    EqualFunc func = (EqualFunc)Lookup_Get(ExactLookup, a->type.of);
    if(func == NULL){
        Error(ErrStream->m, FUNCNAME, FILENAME, LINENUMBER, 
            "Type not yet implemented with Exact", NULL);
            goto nonexact;
    }else{
        if(!func(a, b)){
            goto nonexact;
        }
    }
exact:
    return TRUE;
nonexact:
    if(a != NULL && a->type.state & DEBUG){
        Out("^r.NonExact mismatch &\nvs\n&\n^0.", args);
    }
    return FALSE;
}
