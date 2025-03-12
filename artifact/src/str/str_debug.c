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

i64 StrVec_Print(MemCtx *m, StrVec *v, Abstract *a, cls type, boolean extended){
    StrVec *vObj = (StrVec *)as(a, TYPE_STRVEC);
    i64 total = 0;
    if(extended){
        total += StrVec_FmtAdd(m, v, "StrVec<_i4/_i8:\"", vObj->count, vObj->total); 
    }else{
        total += StrVec_FmtAdd(m, v, "^B\""); 
    }

    Iter *it = &vObj->it;
    Iter_Reset(it);
    while((Iter_Next(it) & END) == 0){
        Str *s = (Str *)Iter_Get(it);
        if(s != NULL){
            Str_Print(m, v, (Abstract *)s, type, extended);
            if((it->type.state & FLAG_ITER_LAST) == 0){
                total += StrVec_Add(v, Str_Ref(m, (byte *)", ", 2, 2));
            }
        }
    }
    total += StrVec_FmtAdd(m, v, "\"^b");
    return total;
}

status Str_DebugInit(MemCtx *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_STR, (void *)Str_Print);
    r |= Lookup_Add(m, lk, TYPE_STRVEC, (void *)StrVec_Print);
    return r;
}
