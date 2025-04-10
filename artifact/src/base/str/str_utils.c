#include <external.h>
#include <caneka.h>

Str *NL = NULL;
Lookup *ToStreamLookup;

status StrUtils_Init(MemCh *m){
    if(NL == NULL){
        NL = Str_Ref(m, (byte *)"\n", 1, 2);
    }
    if(ToStreamLookup == NULL){
        ToStreamLookup = Lookup_Make(m, _TYPE_START, NULL, NULL);
        Mem_DebugInit(m, ToStreamLookup);
        Str_DebugInit(m, ToStreamLookup);
    }
    return SUCCESS;
}

Str *Str_FromTyped(MemCh *m, void *v, cls type){
    if(type == 0){
        type = ((Abstract *)v)->type.of;
    }
    if(v == NULL){
        return Str_Ref(m, (byte *)"NULL", 4, 5);
    }else if(type == TYPE_STR){
        return (Str *)v;
    }else if(type == TYPE_STRVEC){
        Str *s = Str_Ref(m, (byte *)"UNKNOWN", 7, 8);
        s->type.state |= ERROR;
        return s;
    }
}

Str *Str_FromAbs(MemCh *m, Abstract *a){
    if(a == NULL){
        return Str_Ref(m, (byte *)"NULL", 4, 5);
    }else{
        return Str_FromTyped(m, (void *)a, a->type.of);
    }
}

StrVec *StrVec_FromAbs(MemCh *m, Abstract *a){
    if(a == NULL){
        StrVec *v = StrVec_Make(m);
        StrVec_Add(v, Str_Ref(m, (byte *)"NULL", 4, 5));
        return v;
    }else if(a->type.of == TYPE_STRVEC){
        return (StrVec *)a;
    }else if(a->type.of == TYPE_STR){
        StrVec *v = StrVec_Make(m);
        StrVec_Add(v, (Str *)a);
        return v;
    }
}
