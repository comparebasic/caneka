#include <external.h>
#include <caneka.h>

static Str **streamLabels = NULL;
static Str **strLabels = NULL;
i64 Bytes_Debug(Buff *bf, byte *start, byte *end){
    i64 total = 0;
    byte *b = start;
    size_t sz = MAX_BASE10+3;
    byte _digitBytes[sz];
    memset(_digitBytes, 0, sz);
    Str digit_s;
    Str_Init(&digit_s, _digitBytes, 0, sz);
    while(b <= end){
        byte c = *b;
        if(c >= 32 && c <= 126){
            total += Buff_Bytes(bf, b, 1);
        }else if(c == '\r'){
            total += Buff_Bytes(bf, (byte *)"\\r", 2);
        }else if(c == '\n'){
            total += Buff_Bytes(bf, (byte *)"\\n", 2);
        }else if(c == '\t'){
            total += Buff_Bytes(bf, (byte *)"\\t", 2);
        }else{
            Str_Add(&digit_s, (byte *)"\\{", 2);
            i64 i = Str_AddI64(&digit_s, (i64)c);
            Str_Add(&digit_s, (byte *)"}", 1);
            total += Buff_Bytes(bf, digit_s.bytes, digit_s.length);
            digit_s.length = 0;
            memset(_digitBytes, 0, sz);
        }
        b++;
    }
    return total;
}

i64 StrLit_Print(Buff *bf, Abstract *a, cls type, word flags){
    StrLit *sl = (StrLit*)as(a, TYPE_BYTES_POINTER); 
    i64 total = 0;
    if(flags & (MORE|DEBUG)){
        total += Buff_Bytes(bf, (byte *)"StrLit<", 7);
    }
    if(flags & MORE){
        Abstract *args[] = {
            (Abstract *)I16_Wrapped(bf->m, sl->type.range),
            NULL
        };
        total += Fmt(bf, "x/$", args);
    }
    if(flags & DEBUG){
        total += Buff_Bytes(bf, (byte *)" ", 1);
        byte *b = (byte *)((void *)sl)+sizeof(RangeType);
        total += Bytes_Debug(bf, b, b+sl->type.range);
    }
    if(flags & (MORE|DEBUG)){
        total += Buff_Bytes(bf, (byte *)">", 1);
    }

    return total;
}


i64 Str_Print(Buff *bf, Abstract *a, cls type, word flags){
    Str *s = (Str*)as(a, TYPE_STR); 
    flags |= s->type.state & DEBUG;

    if((flags & (MORE|DEBUG)) == 0){
        return Buff_Bytes(bf, s->bytes, s->length);
    }

    i64 total = 0;
    word fl = (DEBUG|MORE);
    if((flags & fl) == fl){
        Abstract *args[] = {
            (Abstract *)I16_Wrapped(bf->m, s->length),
            (Abstract *)I16_Wrapped(bf->m, s->alloc),
            NULL
        };
        total += Buff_Bytes(bf, (byte *)"Str<", 4);
        total += ToS_FlagLabels(bf, (Abstract *)s);
        total += Buff_Bytes(bf, (byte *)" ", 1);
        byte bstr[MAX_BASE10];
        /* lengths are manual here brcause integer wrappers use Str_Print */
        byte *b = bstr;
        i64 len = Str_I64OnBytes(&b, (i64)s->length);
        total += Buff_Bytes(bf, b, len); 
        total += Buff_Bytes(bf, (byte *)"/", 1); 
        b = bstr;
        len = Str_I64OnBytes(&b, (i64)s->alloc);
        total += Buff_Bytes(bf, b, len); 
        total += Buff_Bytes(bf, (byte *)":", 1); 
    }
    if(flags & MORE){
        total += Fmt(bf, "\"^D", NULL); 
    }

    if(flags & DEBUG){
        total += Bytes_Debug(bf, s->bytes, s->bytes+(s->length-1));
    }else{
        total += Buff_Bytes(bf, s->bytes, s->length);
    }

    if((flags & fl) == fl){
        total += Fmt(bf, "^d.\">", NULL);
    }else if(flags & MORE){
        total += Fmt(bf, "^d.\"", NULL);
    }

    return total;
}

i64 StrVec_Print(Buff *bf, Abstract *a, cls type, word flags){
    StrVec *vObj = (StrVec *)as(a, TYPE_STRVEC);
    i64 total = 0;
    if(flags & MORE){
        total += Buff_Bytes(bf, (byte *)"\"", 1); 
    }

    if(flags & DEBUG){
        Abstract *args[] = {
            (Abstract *)StreamTask_Make(bf->m, NULL, (Abstract *)vObj, ToS_FlagLabels),
            (Abstract *)I32_Wrapped(bf->m, vObj->p->nvalues),
            (Abstract *)I64_Wrapped(bf->m, vObj->total),
            NULL
        };
        total += Fmt(bf, "StrVec<$ $/$ [", args); 
    }

    Iter it;
    Iter_Init(&it, vObj->p);
    while((Iter_Next(&it) & END) == 0){
        Str *s = (Str *)it.value;
        if(s != NULL){
            if((flags & (MORE|DEBUG)) == (MORE|DEBUG)){
                Abstract *args[] = {
                    (Abstract *)I32_Wrapped(bf->m, it.idx),
                    NULL
                };
                total += Fmt(bf, "$: ", args); 
                total += Str_Print(bf, (Abstract *)s, type, flags|DEBUG);
                if((it.type.state & LAST) == 0){
                    total += Buff_Bytes(bf, (byte *)", ", 2);
                }
            }else if(flags & DEBUG){
                total += Str_Print(bf, (Abstract *)s, type, flags);
            }else{
                total += Buff_Bytes(bf, s->bytes, s->length);
            }
        }
    }
    if(flags & DEBUG){
        total += Fmt(bf, "]>", NULL);
    }
    if(flags & MORE){
        total += Buff_Bytes(bf, (byte *)"\"",1);
    }
    return total;
}

i64 Cursor_Print(Buff *bf, Abstract *a, cls type, word flags){
    Cursor *curs = (Cursor *)as(a, TYPE_CURSOR);
    if((flags & (MORE|DEBUG)) == 0){
        return ToStream_NotImpl(bf, a, type, flags);
    }

    if(curs->v == NULL){
        return  Fmt(bf, "Curs<v:NULL>", NULL);
    }

    Iter it;
    Iter_Init(&it, curs->v->p);
    i64 pos = 0;
    if(curs->ptr != NULL){
        while((Iter_Next(&it) & END) == 0){
            Str *s = it.value;
            if(it.idx == curs->it.idx){
                pos += (i64)(curs->ptr - s->bytes);
                break;
            }
            pos += (i64)s->length;
        }
    }

    i32 preview = 8;
    if(pos < 8){
        preview = min(pos-1, 0);
    }
    
    i64 length = (i64)(curs->end - curs->ptr)+1;
    i64 endPos = 0;
    StrVec *before = NULL;
    StrVec *focus = curs->v;
    StrVec *after = NULL;
    if((curs->type.state & PROCESSING) && curs->ptr != NULL){
        endPos = pos+length;
        before = Cursor_Get(bf->m, curs, min(0, -preview), 0);
        focus = Cursor_Get(bf->m, curs, 1, 0);
        after = Cursor_Get(bf->m, curs, min(32, endPos - pos), 1);
    }

    if(flags & DEBUG){
        Abstract *args[] = {
            (Abstract *)StreamTask_Make(bf->m, NULL, (Abstract *)curs, ToS_FlagLabels),
            (Abstract *)I64_Wrapped(bf->m, pos),
            (Abstract *)I64_Wrapped(bf->m, endPos),
            (Abstract *)I64_Wrapped(bf->m, length),
            (Abstract *)I64_Wrapped(bf->m, curs->v->total),
            (Abstract *)before,
            (Abstract *)focus,
            (Abstract *)after,
            NULL
        };
        return  Fmt(bf, "Curs<$ $..$ $of$ ^d.: \"$^D.$^d.$\">", args); 
    }else{
        i64 length = (i64)(curs->end - curs->ptr)+1;

        Abstract *args[] = {
            (Abstract *)StreamTask_Make(bf->m, NULL, (Abstract *)curs, ToS_FlagLabels),
            (Abstract *)I64_Wrapped(bf->m, pos),
            (Abstract *)I64_Wrapped(bf->m, curs->v->total),
            (Abstract *)((pos >= 8) ? Str_CstrRef(bf->m, "...") : Str_CstrRef(bf->m, "")),
            (Abstract *)before,
            (Abstract *)focus,
            (Abstract *)after,
            (Abstract *)((endPos < curs->v->total) ? Str_CstrRef(bf->m, "...") 
                : Str_CstrRef(bf->m, "")),
            NULL
        };

        return  Fmt(bf, "Curs<$ $/$ ^D.\"$$^EI.$^ei.$$\"^id.>", args);
    }
}

status Str_InitLabels(MemCh *m, Lookup *lk){
    status r = READY;
    if(streamLabels == NULL){
        streamLabels = (Str **)Arr_Make(m, 17);
        streamLabels[9] = Str_CstrRef(m, "STREAM_STRVEC");
        streamLabels[10] = Str_CstrRef(m, "STREAM_APPEND");
        streamLabels[11] = Str_CstrRef(m, "STREAM_FROM_FD");
        streamLabels[12] = Str_CstrRef(m, "STREAM_TO_FD");
        streamLabels[13] = Str_CstrRef(m, "STREAM_MESS");
        streamLabels[14] = Str_CstrRef(m, "STREAM_SOCKET");
        streamLabels[15] = Str_CstrRef(m, "STREAM_BUFFER");
        Lookup_Add(m, lk, TYPE_STREAM, (void *)streamLabels);
        r |= SUCCESS;
    }
    if(strLabels == NULL){
        strLabels = (Str **)Arr_Make(m, 17);
        strLabels[9] = Str_CstrRef(m, "TEXT");
        strLabels[10] = Str_CstrRef(m, "CONST");
        strLabels[11] = Str_CstrRef(m, "BINARY");
        strLabels[12] = Str_CstrRef(m, "ENCODED");
        strLabels[13] = Str_CstrRef(m, "FMT_ANSI");
        strLabels[14] = Str_CstrRef(m, "COPY");
        strLabels[15] = Str_CstrRef(m, "UTF8");
        strLabels[16] = Str_CstrRef(m, "SEP");
        Lookup_Add(m, lk, TYPE_STR, (void *)strLabels);
        r |= SUCCESS;
    }
    return r;
}

status Str_ToSInit(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_STR, (void *)Str_Print);
    r |= Lookup_Add(m, lk, TYPE_STRVEC, (void *)StrVec_Print);
    r |= Lookup_Add(m, lk, TYPE_CURSOR, (void *)Cursor_Print);
    r |= Lookup_Add(m, lk, TYPE_BYTES_POINTER, (void *)StrLit_Print);
    r |= Str_InitLabels(m, ToSFlagLookup);
    return r;
}
