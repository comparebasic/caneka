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
    }
    return FALSE;
}
