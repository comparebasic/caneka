#include <external.h>
#include <caneka.h>

boolean Exact(Abstract *a, Abstract *b){
    if(a == NULL || b == NULL){
        return FALSE;
    }else if(a == b){
        return TRUE;
    }else if(a->type.of != b->type.of){
        return FALSE;
    }else if(a->type.of == TYPE_STR){
        return Str_ExactStr((Str *)a, (Str *)b);
    }else if(a->type.of == TYPE_STRVEC){
        return StrVec_ExactStrVec((StrVec *)a, (StrVec *)b);
    }else if(a->type.of == TYPE_SPAN){
        Error(ErrStream->m, a, FUNCNAME, FILENAME, LINENUMBER, 
            "Type not yet implemented with Exact", NULL);
    }else if(a->type.of == TYPE_WRAPPED_PTR){
        Error(ErrStream->m, a, FUNCNAME, FILENAME, LINENUMBER, 
            "Type not yet implemented with Exact", NULL);
    }
    Error(ErrStream->m, a, FUNCNAME, FILENAME, LINENUMBER, 
        "Type not yet implemented with Exact", NULL);
    return FALSE;
}
