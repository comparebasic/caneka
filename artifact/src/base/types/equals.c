#include <external.h>
#include <caneka.h>

Lookup *EqualsLookup = NULL;

status Equals_Init(MemCh *m){
    status r = READY;
    if(EqualsLookup == NULL){
        EqualsLookup = Lookup_Make(m, _TYPE_ZERO, NULL, NULL);
        Mem_EqInit(m, EqualsLookup);
        Sequence_EqInit(m, EqualsLookup);
        Util_EqInit(m, EqualsLookup);
        r |= SUCCESS;
    }
    return r;
}

boolean Equals(Abstract *a, Abstract *b){
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
    }else{
        cls aTypeOf = Ifc_Get(a->type.of);
        cls bTypeOf = Ifc_Get(b->type.of);
        if(aTypeOf != bTypeOf){
            Abstract *args[] = {
                (Abstract *)Type_ToStr(ErrStream->m, aTypeOf),
                (Abstract *)Type_ToStr(ErrStream->m, bTypeOf),
                NULL
            };
            Error(ErrStream->m, a, FUNCNAME, FILENAME, LINENUMBER, 
                "Equals type mismatche $ vs $", args);
        }else{
            EqFunc func = (EqFunc)Lookup_Get(EqualsLookup, aTypeOf);
            if(func == NULL){
                Abstract *args[] = {
                    (Abstract *)Type_ToStr(ErrStream->m, aTypeOf),
                    NULL
                };
                Error(ErrStream->m, a, FUNCNAME, FILENAME, LINENUMBER, 
                    "Equals for $ not registered", args);
            }else{
                return func(a, b);
            }
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


