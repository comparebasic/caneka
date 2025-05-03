#include <external.h>
#include <caneka.h>

i64 Str_Print(Stream *sm, Abstract *a, cls type, word flags){
    Str *s = (Str*)as(a, TYPE_STR); 
    if((flags & (MORE|DEBUG)) == 0){
        return Stream_Bytes(sm, s->bytes, s->length);
    }

    i64 total = 0;
    size_t sz = MAX_BASE10+3;
    byte _digitBytes[sz];
    memset(_digitBytes, 0, sz);
    Str digit_s;
    Str_Init(&digit_s, _digitBytes, 0, sz);

    if(flags & DEBUG){
        Abstract *args[] = {
            (Abstract *)I16_Wrapped(sm->m, s->length),
            (Abstract *)I16_Wrapped(sm->m, s->alloc),
            NULL
        };
        total += Fmt(sm, "Str<$/$:^D\"", args); 
    }else if(flags & MORE){
        total += Fmt(sm, "^D\"", NULL); 
    }

    if(flags & DEBUG){
        byte *b = s->bytes;
        byte *end = s->bytes+(s->length-1);
        while(b <= end){
            byte c = *b;
            if(c >= 32 && c <= 126){
                total += Stream_Bytes(sm, b, 1);
            }else if(c == '\r'){
                total += Stream_Bytes(sm, (byte *)"\\r", 2);
            }else if(c == '\n'){
                total += Stream_Bytes(sm, (byte *)"\\n", 2);
            }else if(c == '\t'){
                total += Stream_Bytes(sm, (byte *)"\\t", 2);
            }else{
                Str_Add(&digit_s, (byte *)"\\{", 2);
                i64 i = Str_AddI64(&digit_s, (i64)c);
                Str_Add(&digit_s, (byte *)"}", 1);
                total += Stream_Bytes(sm, digit_s.bytes, digit_s.length);
                digit_s.length = 0;
                memset(_digitBytes, 0, sz);
            }
            b++;
        }
    }else{
        total += Stream_Bytes(sm, s->bytes, s->length);
    }

    if(flags & DEBUG){
        total += Fmt(sm, "\"^d.>", NULL);
    }else if(flags & MORE){
        total += Fmt(sm, "\"^d.", NULL);
    }

    return total;
}

i64 StrVec_Print(Stream *sm, Abstract *a, cls type, word flags){
    StrVec *vObj = (StrVec *)as(a, TYPE_STRVEC);
    i64 total = 0;
    if(flags & DEBUG){
        Abstract *args[] = {
            (Abstract *)I32_Wrapped(sm->m, vObj->p->nvalues),
            (Abstract *)I64_Wrapped(sm->m, vObj->total),
            NULL
        };
        total += Fmt(sm, "StrVec<$/$ [", args); 
    }else if(flags & MORE){
        total += Fmt(sm, "^D", NULL); 
    }

    Iter it;
    Iter_Init(&it, vObj->p);
    while((Iter_Next(&it) & END) == 0){
        Str *s = (Str *)it.value;
        if(s != NULL){
            if((flags & (MORE|DEBUG)) == (MORE|DEBUG)){
                Abstract *args[] = {
                    (Abstract *)I32_Wrapped(sm->m, it.idx),
                    NULL
                };
                total += Fmt(sm, "$: ", args); 
                total += Str_Print(sm, (Abstract *)s, type, flags);
                if((it.type.state & FLAG_ITER_LAST) == 0){
                    total += Stream_Bytes(sm, (byte *)", ", 2);
                }
            }else{
                total += Str_Print(sm, (Abstract *)s, type, flags);
            }
        }
    }
    if(flags & DEBUG){
        total += Fmt(sm, "]^d>", NULL);
    }else if (flags & MORE){
        total += Fmt(sm, "^d", NULL);
    }
    return total;
}

i64 Stream_Print(Stream *sm, Abstract *a, cls type, word flags){
    Stream *smObj = (Stream *)as(a, TYPE_STREAM);
    if(flags & (MORE|DEBUG)){
        i64 total = 0;
        total += Fmt(sm, "Stream<", NULL); 
        if(sm->type.state & STREAM_STRVEC){
            total += StrVec_Print(sm, (Abstract *)smObj->dest.curs->v, type, flags);
        }
        total += Fmt(sm, ">", NULL); 
        return total;
    }else{
        return ToStream_NotImpl(sm, a, type, flags);
    }
}

i64 Cursor_Print(Stream *sm, Abstract *a, cls type, word flags){
    Cursor *curs = (Cursor *)as(a, TYPE_CURSOR);
    if((flags & (MORE|DEBUG)) == 0){
        return ToStream_NotImpl(sm, a, type, flags);
    }

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

    if(flags & DEBUG){
        i64 length = (i64)(curs->end - curs->ptr);
        i64 endPos = pos+length;
        Single *val = Ptr_Wrapped(sm->m, curs->v, 0);
        val->type.state |= DEBUG;
        Abstract *args[] = {
            (Abstract *)I64_Wrapped(sm->m, pos),
            (Abstract *)I64_Wrapped(sm->m, endPos),
            (Abstract *)I64_Wrapped(sm->m, length),
            (Abstract *)I64_Wrapped(sm->m, curs->v->total),
            (Abstract *)Str_Ref(sm->m, curs->ptr, length, length+1, 0),
            (Abstract *)val,
            NULL
        };
        return  Fmt(sm, "Curs<$..$ $of$ ^D.'@'^d. $>", args); 
    }else{
        i64 length = (i64)(curs->end - curs->ptr);
        Abstract *args[] = {
            (Abstract *)I64_Wrapped(sm->m, pos),
            (Abstract *)I64_Wrapped(sm->m, curs->v->total),
            (Abstract *)Str_Ref(sm->m, curs->ptr, length, length+1, 0),
            NULL
        };

        return  Fmt(sm, "Curs<$/$_^D.'@'^d.>", args); 
    }
}

status Str_ToSInit(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_STR, (void *)Str_Print);
    r |= Lookup_Add(m, lk, TYPE_STRVEC, (void *)StrVec_Print);
    r |= Lookup_Add(m, lk, TYPE_STREAM, (void *)Stream_Print);
    r |= Lookup_Add(m, lk, TYPE_CURSOR, (void *)Cursor_Print);
    return r;
}
