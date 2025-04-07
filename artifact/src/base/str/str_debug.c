#include <external.h>
#include <caneka.h>

#include "inline/handle_io.c"

i64 Str_Print(MemCh *m, StrVec *v, i32 fd, Abstract *a, cls type, boolean extended){
    Str *s = (Str*)as(a, TYPE_STR); 
    i64 total = 0;
    if(extended){
        total += StrVec_FmtAdd(m, v, fd, "Str<_i4/_i4:^D\"", s->length, s->alloc); 
    }else{
        total += StrVec_FmtAdd(m, v, fd, "^D\""); 
    }
    total += handleIo(v, fd, s);
    if(extended){
        total += StrVec_FmtAdd(m, v, fd, "\"^d.>");
    }else{
        total += StrVec_FmtAdd(m, v, fd, "\"^d.");
    }
    return total;
}

i64 StrVec_Print(MemCh *m, StrVec *v, i32 fd, Abstract *a, cls type, boolean extended){
    StrVec *vObj = (StrVec *)as(a, TYPE_STRVEC);
    i64 total = 0;
    if(extended){
        total += StrVec_FmtAdd(m, v, fd, "StrVec<_i4/_i8:\"", vObj->p->nvalues, vObj->total); 
    }else{
        total += StrVec_FmtAdd(m, v, fd, "^D\""); 
    }

    Iter it;
    Iter_Init(&it, vObj->p);
    while((Iter_Next(&it) & END) == 0){
        Str *s = (Str *)it.value;
        if(s != NULL){
            Str_Print(m, v, fd, (Abstract *)s, type, extended);
            if((it.type.state & FLAG_ITER_LAST) == 0){
                total += handleIo(v, fd, Str_Ref(m, (byte *)", ", 2, 2));
            }
        }
    }
    if(extended){
        total += StrVec_FmtAdd(m, v, fd, "\"^d>");
    }else{
        total += StrVec_FmtAdd(m, v, fd, "\"^d");
    }
    return total;
}

i64 Stream_Print(MemCh *m, StrVec *v, i32 fd, Abstract *a, cls type, boolean extended){
    Stream *sm = (Stream *)as(a, TYPE_STREAM);
    i64 total = 0;
    total += StrVec_FmtAdd(m, v, fd, "Stream<"); 
    if(sm->type.state & STREAM_STRVEC){
        total += StrVec_Print(m, v, fd, (Abstract *)sm->dest.curs->v, type, extended);
    }
    total += StrVec_FmtAdd(m, v, fd, ">"); 
    return total;
}

status Str_DebugInit(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_STR, (void *)Str_Print);
    r |= Lookup_Add(m, lk, TYPE_STRVEC, (void *)StrVec_Print);
    r |= Lookup_Add(m, lk, TYPE_STREAM, (void *)Stream_Print);
    return r;
}
