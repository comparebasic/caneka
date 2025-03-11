#include <external.h>
#include <caneka.h>

i64 Str_Print(MemCtx *m, StrVec *v, Abstract *a, cls type, boolean extended){
    Str *s = (Str*)as(a, TYPE_STR); 
    i64 total = 0;
    if(extended){
        total += StrVec_FmtAdd(m, v, "Str<_i4/_i4:^B\"", s->length, s->alloc); 
    }else{
        total += StrVec_FmtAdd(m, v, "^B\""); 
    }
    total += StrVec_Add(v, s); 
    if(extended){
        total += StrVec_FmtAdd(m, v, "\"^b^>");
    }else{
        total += StrVec_FmtAdd(m, v, "\"^b");
    }
    return total;
}

status Str_DebugInit(MemCtx *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_STR, (void *)Str_Print);
    return r;
}
