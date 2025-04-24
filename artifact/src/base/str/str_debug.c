#include <external.h>
#include <caneka.h>

i64 Str_Print(Stream *sm, Abstract *a, cls type, boolean extended){
    Str *s = (Str*)as(a, TYPE_STR); 
    i64 total = 0;
    if(extended){
        Abstract *args[] = {
            (Abstract *)I16_Wrapped(sm->m, s->length),
            (Abstract *)I16_Wrapped(sm->m, s->alloc),
            NULL
        };
        total += Fmt(sm, "Str<$/$:^D\"", args); 
    }else{
        total += Fmt(sm, "^D\"", NULL); 
    }
    total += Stream_To(sm, s->bytes, s->length);
    if(extended){
        total += Fmt(sm, "\"^d.>", NULL);
    }else{
        total += Fmt(sm, "\"^d.", NULL);
    }
    return total;
}

i64 StrVec_Print(Stream *sm, Abstract *a, cls type, boolean extended){
    StrVec *vObj = (StrVec *)as(a, TYPE_STRVEC);
    i64 total = 0;
    if(extended){
        Abstract *args[] = {
            (Abstract *)I32_Wrapped(sm->m, vObj->p->nvalues),
            (Abstract *)I64_Wrapped(sm->m, vObj->total),
            NULL
        };
        total += Fmt(sm, "StrVec<$/$ [", args); 
    }else{
        total += Fmt(sm, "^D", NULL); 
    }

    Iter it;
    Iter_Init(&it, vObj->p);
    while((Iter_Next(&it) & END) == 0){
        Str *s = (Str *)it.value;
        if(s != NULL){
            Abstract *args[] = {
                (Abstract *)I32_Wrapped(sm->m, it.idx),
                NULL
            };
            total += Fmt(sm, "$: ", args); 
            Str_Print(sm, (Abstract *)s, type, extended);
            if((it.type.state & FLAG_ITER_LAST) == 0){
                total += Stream_To(sm, (byte *)", ", 2);
            }
            if((it.type.state & FLAG_ITER_LAST) == 0){
                total += Stream_To(sm, (byte *)", ", 2); 
            }
        }
    }
    if(extended){
        total += Fmt(sm, "]^d>", NULL);
    }else{
        total += Fmt(sm, "^d", NULL);
    }
    return total;
}

i64 Stream_Print(Stream *sm, Abstract *a, cls type, boolean extended){
    Stream *smObj = (Stream *)as(a, TYPE_STREAM);
    i64 total = 0;
    total += Fmt(sm, "Stream<", NULL); 
    if(sm->type.state & STREAM_STRVEC){
        total += StrVec_Print(sm, (Abstract *)smObj->dest.curs->v, type, extended);
    }
    total += Fmt(sm, ">", NULL); 
    return total;
}

i64 Cursor_Print(Stream *sm, Abstract *a, cls type, boolean extended){
    Cursor *curs = (Cursor *)as(a, TYPE_CURSOR);
    if(curs->v == NULL){
        return  Fmt(sm, "Curs<v:NULL>", NULL); 
    }

    Iter it;
    Iter_Init(&it, curs->v->p);
    i64 pos = 0;
    while((Iter_Next(&it) & END) == 0){
        Str *s = it.value;
        if(it.idx == curs->it.idx){
            pos += (i64)(curs->ptr - s->bytes);
            break;
        }
        pos += s->length;
    }

    if(extended){
        i64 length = (i64)(curs->end - curs->ptr);
        i64 endPos = pos+length;
        Single *val = Ptr_Wrapped(sm->m, curs->v);
        val->type.state |= DEBUG;
        Abstract *args[] = {
            (Abstract *)I64_Wrapped(sm->m, pos),
            (Abstract *)I64_Wrapped(sm->m, endPos),
            (Abstract *)I64_Wrapped(sm->m, length),
            (Abstract *)I64_Wrapped(sm->m, curs->v->total),
            (Abstract *)Str_Ref(sm->m, curs->ptr, length, length+1),
            (Abstract *)val,
            NULL
        };
        return  Fmt(sm, "Curs<$..$ $of$ ^D.'$'^d. $>", args); 
    }else{
        i64 length = (i64)(curs->end - curs->ptr);
        Abstract *args[] = {
            (Abstract *)I64_Wrapped(sm->m, pos),
            (Abstract *)I64_Wrapped(sm->m, curs->v->total),
            (Abstract *)Str_Ref(sm->m, curs->ptr, length, length+1),
            NULL
        };

        return  Fmt(sm, "Curs<$/$_^D.'$'^d.>", args); 
    }
}

status Str_DebugInit(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_STR, (void *)Str_Print);
    r |= Lookup_Add(m, lk, TYPE_STRVEC, (void *)StrVec_Print);
    r |= Lookup_Add(m, lk, TYPE_STREAM, (void *)Stream_Print);
    r |= Lookup_Add(m, lk, TYPE_CURSOR, (void *)Cursor_Print);
    return r;
}
