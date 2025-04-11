#include <external.h>
#include <caneka.h>

i64 Str_Print(Stream *sm, Abstract *a, cls type, boolean extended){
    Str *s = (Str*)as(a, TYPE_STR); 
    i64 total = 0;
    if(extended){
        total += StrVec_Fmt(sm, "Str<_i4/_i4:^D\"", s->length, s->alloc); 
    }else{
        total += StrVec_Fmt(sm, "^D\""); 
    }
    total += Stream_To(sm, s->bytes, s->length);
    if(extended){
        total += StrVec_Fmt(sm, "\"^d.>");
    }else{
        total += StrVec_Fmt(sm, "\"^d.");
    }
    return total;
}

i64 StrVec_Print(Stream *sm, Abstract *a, cls type, boolean extended){
    StrVec *vObj = (StrVec *)as(a, TYPE_STRVEC);
    i64 total = 0;
    if(extended){
        total += StrVec_Fmt(sm, "StrVec<_i4/_i8:\"", vObj->p->nvalues, vObj->total); 
    }else{
        total += StrVec_Fmt(sm, "^D\""); 
    }

    Iter it;
    Iter_Init(&it, vObj->p);
    while((Iter_Next(&it) & END) == 0){
        Str *s = (Str *)it.value;
        if(s != NULL){
            total += StrVec_Fmt(sm, "_i4: ", it.idx); 
            Str_Print(sm, (Abstract *)s, type, extended);
            if((it.type.state & FLAG_ITER_LAST) == 0){
                total += Stream_To(sm, (byte *)", ", 2);
            }
            if((it.type.state & FLAG_ITER_LAST) == 0){
                total += StrVec_Fmt(sm, ", "); 
            }
        }
    }
    if(extended){
        total += StrVec_Fmt(sm, "\"^d>");
    }else{
        total += StrVec_Fmt(sm, "\"^d");
    }
    return total;
}

i64 Stream_Print(Stream *sm, Abstract *a, cls type, boolean extended){
    Stream *smObj = (Stream *)as(a, TYPE_STREAM);
    i64 total = 0;
    total += StrVec_Fmt(sm, "Stream<"); 
    if(sm->type.state & STREAM_STRVEC){
        total += StrVec_Print(sm, (Abstract *)smObj->dest.curs->v, type, extended);
    }
    total += StrVec_Fmt(sm, ">"); 
    return total;
}

status Str_DebugInit(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_STR, (void *)Str_Print);
    r |= Lookup_Add(m, lk, TYPE_STRVEC, (void *)StrVec_Print);
    r |= Lookup_Add(m, lk, TYPE_STREAM, (void *)Stream_Print);
    return r;
}
