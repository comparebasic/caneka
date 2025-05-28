#include <external.h>
#include <caneka.h>

boolean Equals(Abstract *a, Abstract *b){
    if(a == NULL || b == NULL){
        return FALSE;
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
    }else if(a->type.of == b->type.of){
        if(a->type.of == TYPE_MESS){
            return Equals((Abstract *)((Mess *)a)->root,
                (Abstract *)((Mess *)b)->root);
        }else if(a->type.of == TYPE_NODE){
            Node *nA = (Node *)a; 
            Node *nB = (Node *)b; 
            if(nA->captureKey != nB->captureKey || 
                    nA->typeOfChild != nB->typeOfChild){
                return FALSE;
            }else{
                if((nA->value != NULL && nB->value != NULL) &&
                    !Equals(nA->value, nB->value)){
                        return FALSE;
                }else if((nA->atts != NULL && nB->atts != NULL) &&
                    !Equals((Abstract *)nA->atts, (Abstract *)nB->atts)){
                        return FALSE;
                }else{
                    return Equals(nA->child, nB->child);
                }
            }
        }else if(a->type.of == TYPE_SPAN){
            return FALSE;
        }else if(a->type.of == TYPE_TABLE){
            return FALSE;
        }else if(a->type.of == TYPE_RELATION){
            return FALSE;
        }
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
    }else if(Ifc_Match(a->type.of, TYPE_STR)){
        return ((Str *)a)->length > 1;
    }else{
        return FALSE;
    }
}


