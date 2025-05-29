#include <external.h>
#include <caneka.h>

Lookup *EqualsLookup = NULL;

status Equals_Init(MemCh *m){
    status r = READY;
    if(EqualsLookup == NULL){
        EqualsLookup = Lookup_Make(m, _TYPE_ZERO, NULL, NULL);
        Mem_EqInit(m, EqualsLookup);
        Sequence_EqInit(m, EqualsLookup);
        r |= SUCCESS;
    }
    return r;
}

boolean Equals(Abstract *a, Abstract *b){
    if(a == NULL || b == NULL){
        return FALSE;
    }if( a == b){
        return TRUE;
    }else if(a->type.of == TYPE_STR){
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
    }else if(a->type.of == TYPE_WRAPPED_I16 && a->type.of == b->type.of){
        Single *wa = (Single *)a;
        Single *wb = (Single *)b;
        return wa->val.w == wb->val.w;
    }else if(a->type.of == b->type.of){
        EqFunc func = (EqFunc)Lookup_Get(EqualsLookup, a->type.of);
        if(func != NULL){
            return func(a, b);
        }

    }
    return FALSE;
}

boolean Caneka_Truthy(Abstract *a){
    if(a == NULL){
        return FALSE;
    }else if(Ifc_Match(a->type.of, TYPE_WRAPPED_I64)){
        return ((Single *)a)->val.value != 0;
    }else if(Ifc_Match(a->type.of, TYPE_STR)){
        return ((Str *)a)->length > 1;
    }else{
        return FALSE;
    }
}


