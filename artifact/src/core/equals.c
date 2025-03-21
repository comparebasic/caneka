#include <external.h>
#include <caneka.h>

boolean Equals(Abstract *a, Abstract *b){
    if(a->type.of == TYPE_STR){
        if(a->type.of == b->type.of){
            return Str_EqualsStr((Str *)a, (Str *)b);
        }else if (b->type.of == TYPE_STRVEC){
            return Str_EqualsStrVec((Str *)a, (StrVec *)b);
        }
    }else if(a->type.of == TYPE_STRVEC){
        if(a->type.of == b->type.of){
            return StrVec_EqualsStrVec((StrVec *)a, (StrVec *)b);
        }else if(b->type.of == TYPE_STR){
            return Str_EqualsStrVec((Str *)b, (StrVec *)a);
        }
    }else if(a->type.of == TYPE_WRAPPED_UTIL || a->type.of == TYPE_WRAPPED_I64){
        Single *wa = (Single *)a;
        Single *wb = (Single *)b;
        return wa->val.value == wb->val.value;
    }else{
        return ((void *)a) == b;
    }
    return FALSE;
}

boolean Caneka_Truthy(Abstract *a){
    if(a == NULL){
        return FALSE;
    }else if(Ifc_Match(a->type.of, TYPE_WRAPPED_I64)){
        return ((Single *)a)->val.value != 0;
    }else if(Ifc_Match(a->type.of, TYPE_STRING)){
        return ((Str *)a)->length > 1;
    }else{
        return FALSE;
    }
}


