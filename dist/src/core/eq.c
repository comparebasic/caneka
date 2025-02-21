#include <external.h>
#include <caneka.h>

boolean Abs_Eq(Abstract *a, void *b){
    if(a == NULL || b == NULL){
        return FALSE;
    }
    if(a->type.of == TYPE_STRING_CHAIN || a->type.of == TYPE_STRING_FIXED){
        return String_Eq(a, b);
    }else{
        return ((void *)a) == b;
    }
}

boolean Caneka_Truthy(Abstract *a){
    if(a == NULL){
        return FALSE;
    }else if(Ifc_Match(a->type.of, TYPE_WRAPPED_I64)){
        return ((Single *)a)->val.value != 0;
    }else if(Ifc_Match(a->type.of, TYPE_STRING)){
        return ((String *)a)->length > 1;
    }else{
        return FALSE;
    }
}

