#include <external.h>
#include "base_module.h"

Lookup *EqualsLookup = NULL;

status Equals_Init(MemCh *m){
    status r = READY;
    if(EqualsLookup == NULL){
        EqualsLookup = Lookup_Make(m, _TYPE_ZERO);
        Mem_EqInit(m, EqualsLookup);
        Sequence_EqInit(m, EqualsLookup);
        Util_EqInit(m, EqualsLookup);
        r |= SUCCESS;
    }
    return r;
}

boolean Equals(void *_a, void *_b){
    Abstract *a = (Abstract *)_a;
    Abstract *b = (Abstract *)_b;
    if(a == NULL || b == NULL){
        return FALSE;
    }else if( a == b){
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
    }else if(a->type.of == TYPE_SPAN && a->type.of == b->type.of){
        Span *pa = (Span *)a;
        Span *pb = (Span *)b;
        return Span_Equals(pa, pb);
    }else if(
            (a->type.of > _TYPE_WRAPPED_START && 
                a->type.of < _TYPE_WRAPPED_END) &&
            (b->type.of > _TYPE_WRAPPED_START && 
                b->type.of < _TYPE_WRAPPED_END)
        ){
        Single *sga = (Single *)a;
        Single *sgb = (Single *)b;

        util au;
        util bu;

        if(a->type.of == TYPE_WRAPPED_I64 || a->type.of == TYPE_WRAPPED_UTIL){
            au = sga->val.value;
        }else if((a->type.of == TYPE_WRAPPED_I32 || a->type.of == TYPE_WRAPPED_U32)){
            au = sga->val.i;
        }else if((a->type.of == TYPE_WRAPPED_I16 || a->type.of == TYPE_WRAPPED_WORD)){
            au = sga->val.w;
        }else if((a->type.of == TYPE_WRAPPED_I8 || a->type.of == TYPE_WRAPPED_BYTE)){
            au = sga->val.b;
        }

        if(b->type.of == TYPE_WRAPPED_I64 || b->type.of == TYPE_WRAPPED_UTIL){
            bu = sgb->val.value;
        }else if((b->type.of == TYPE_WRAPPED_I32 || b->type.of == TYPE_WRAPPED_U32)){
            bu = sgb->val.i;
        }else if((b->type.of == TYPE_WRAPPED_I16 || b->type.of == TYPE_WRAPPED_WORD)){
            bu = sgb->val.w;
        }else if((b->type.of == TYPE_WRAPPED_I8 || b->type.of == TYPE_WRAPPED_BYTE)){
            bu = sgb->val.b;
        }

        return au == bu;
    }else{
        cls aTypeOf = Ifc_Get(a->type.of);
        cls bTypeOf = Ifc_Get(b->type.of);
        if(aTypeOf != bTypeOf){
            void *args[] = {
                Type_ToStr(ErrStream->m, aTypeOf),
                Type_ToStr(ErrStream->m, bTypeOf),
                NULL
            };
            Error(ErrStream->m, FUNCNAME, FILENAME, LINENUMBER, 
                "Equals type mismatche $ vs $", args);
        }else{
            EqFunc func = (EqFunc)Lookup_Get(EqualsLookup, aTypeOf);
            if(func == NULL){
                void *args[] = {
                    Type_ToStr(ErrStream->m, aTypeOf),
                    NULL
                };
                Error(ErrStream->m, FUNCNAME, FILENAME, LINENUMBER, 
                    "Equals for $ not registered", args);
            }else{
                return func(a, b);
            }
        }
    }
    return FALSE;
}

boolean Caneka_Truthy(void *_a){
    Abstract *a = (Abstract *)_a;
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


