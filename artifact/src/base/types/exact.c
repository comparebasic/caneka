#include <external.h>
#include <caneka.h>

boolean Exact(Abstract *a, Abstract *b){
    Abstract *args[] = {
        a, b, NULL
    };
    if(a == NULL || b == NULL){
        goto nonexact;
    }else if(a == b){
        goto exact;
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
    Error(ErrStream->m, a, FUNCNAME, FILENAME, LINENUMBER, 
        "Type not yet implemented with Exact", NULL);

exact:
    return TRUE;
nonexact:
    if(a->type.state & DEBUG){
        Out("^r.NonExact mismatch &\nvs\n&\n^0.", args);
    }
    return FALSE;
}
