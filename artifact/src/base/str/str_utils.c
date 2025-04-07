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
    if(v == NULL){
        return Str_Ref(m, (byte *)"NULL", 4, 5);
    }else if(type == TYPE_STR){
        return (Str *)v;
    }else{
        Str *s = Str_Ref(m, (byte *)"UNKNOWN", 4, 5);
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
