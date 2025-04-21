#include <external.h>
#include <caneka.h>

i64 Str_Print(Stream *sm, Abstract *a, cls type, boolean extended){
    Str *s = (Str*)as(a, TYPE_STR); 
    i64 total = 0;
    if(extended){
        void *args[] = {&s->length, &s->alloc, NULL};
        total += StrVec_Fmt(sm, "Str<_i2/_i2:^D\"", args); 
    }else{
        total += StrVec_Fmt(sm, "^D\"", NULL); 
    }
    total += Stream_To(sm, s->bytes, s->length);
    if(extended){
        total += StrVec_Fmt(sm, "\"^d.>", NULL);
    }else{
        total += StrVec_Fmt(sm, "\"^d.", NULL);
    }
    return total;
}

i64 StrVec_Print(Stream *sm, Abstract *a, cls type, boolean extended){
    StrVec *vObj = (StrVec *)as(a, TYPE_STRVEC);
    i64 total = 0;
    if(extended){
        void *args[] = { &vObj->p->nvalues, &vObj->total, NULL};
        total += StrVec_Fmt(sm, "StrVec<_i4/_i8:\"", args); 
    }else{
        total += StrVec_Fmt(sm, "^D\"", NULL); 
    }

    Iter it;
    Iter_Init(&it, vObj->p);
    while((Iter_Next(&it) & END) == 0){
        Str *s = (Str *)it.value;
        if(s != NULL){
            void *args[] = {&it.idx, NULL};
            total += StrVec_Fmt(sm, "_i4: ", args); 
            Str_Print(sm, (Abstract *)s, type, extended);
            if((it.type.state & FLAG_ITER_LAST) == 0){
                total += Stream_To(sm, (byte *)", ", 2);
            }
            if((it.type.state & FLAG_ITER_LAST) == 0){
                total += StrVec_Fmt(sm, ", ", NULL); 
            }
        }
    }
    if(extended){
        total += StrVec_Fmt(sm, "\"^d>", NULL);
    }else{
        total += StrVec_Fmt(sm, "\"^d", NULL);
    }
    return total;
}

i64 Stream_Print(Stream *sm, Abstract *a, cls type, boolean extended){
    Stream *smObj = (Stream *)as(a, TYPE_STREAM);
    i64 total = 0;
    total += StrVec_Fmt(sm, "Stream<", NULL); 
    if(sm->type.state & STREAM_STRVEC){
        total += StrVec_Print(sm, (Abstract *)smObj->dest.curs->v, type, extended);
    }
    total += StrVec_Fmt(sm, ">", NULL); 
    return total;
}

status Str_DebugInit(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_STR, (void *)Str_Print);
    r |= Lookup_Add(m, lk, TYPE_STRVEC, (void *)StrVec_Print);
    r |= Lookup_Add(m, lk, TYPE_STREAM, (void *)Stream_Print);
    return r;
}
