#include <external.h>
#include <caneka.h>

Lookup *ToStreamLookup;

status StrUtils_Init(MemCh *m){
    if(ToStreamLookup == NULL){
        ToStreamLookup = Lookup_Make(m, _TYPE_START, NULL, NULL);
        Mem_DebugInit(m, ToStreamLookup);
        Str_DebugInit(m, ToStreamLookup);
    }
    return SUCCESS;
}

StrVec *StrVec_FromAbs(MemCh *m, Abstract *a){
    if(a != NULL){
        if(a->type.of == TYPE_STRVEC){
            return (StrVec *)a;
        }else if(a->type.of == TYPE_STR){
            StrVec *v = StrVec_Make(m);
            StrVec_Add(v, (Str *)a);
            return v;
        }
        StrVec *v = StrVec_Make(m);
        StrVec_Add(v, Str_CstrRef(m, "UNSUPPORTED"));
        return v;
    }
    StrVec *v = StrVec_Make(m);
    StrVec_Add(v, Str_Ref(m, (byte *)"NULL", 4, 5));
    return v;
}
