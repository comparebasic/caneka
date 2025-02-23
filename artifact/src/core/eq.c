#include <external.h>
#include <caneka.h>

boolean Abs_Eq(Abstract *a, void *b){
    cls ifcOf = Ifc_Get(a->type.of);
    if(a == NULL || b == NULL){
        return FALSE;
    }else if(ifcOf != 0 && ifcOf != Ifc_Get(((Abstract *)b)->type.of)){
        return FALSE;
    }

    if(ifcOf == TYPE_STRING){
        return String_Equals((String *)a, (String *)b);
    }else if(a->type.of == TYPE_WRAPPED_UTIL || a->type.of == TYPE_WRAPPED_I64){
        Single *wa = (Single *)a;
        Single *wb = (Single *)b;
        return wa->val.value == wb->val.value;
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

