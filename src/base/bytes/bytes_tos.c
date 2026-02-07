/*
 * ToStream and state label functions for Bytes component 
 */

#include <external.h>
#include "base_module.h"

static Str **streamLabels = NULL;
static Str **strLabels = NULL;
static Str **histoLabels = NULL;

status Bytes_Debug(Buff *bf, byte *start, byte *end){
    byte *b = start;
    size_t sz = MAX_BASE10+3;
    byte _digitBytes[sz];
    memset(_digitBytes, 0, sz);
    Str digit_s;
    Str_Init(&digit_s, _digitBytes, 0, sz);
    while(b <= end){
        byte c = *b;
        if(c >= 32 && c <= 126){
            Buff_AddBytes(bf, b, 1);
        }else if(c == '\r'){
            Buff_AddBytes(bf, (byte *)"\\r", 2);
        }else if(c == '\n'){
            Buff_AddBytes(bf, (byte *)"\\n", 2);
        }else if(c == '\t'){
            Buff_AddBytes(bf, (byte *)"\\t", 2);
        }else{
            Str_Add(&digit_s, (byte *)"\\{", 2);
            i64 i = Str_AddI64(&digit_s, (i64)c);
            Str_Add(&digit_s, (byte *)"}", 1);
            Buff_AddBytes(bf, digit_s.bytes, digit_s.length);
            digit_s.length = 0;
            memset(_digitBytes, 0, sz);
        }
        b++;
    }
    return SUCCESS;
}

status BytesLit_Print(Buff *bf, void *a, cls type, word flags){
    BytesLit *sl = (BytesLit*)as(a, TYPE_BYTES_POINTER); 
    if(flags & (MORE|DEBUG)){
        Buff_AddBytes(bf, (byte *)"BytesLit<", 7);
    }
    if(flags & MORE){
        void *args[] = {
            I16_Wrapped(bf->m, sl->type.range),
            NULL
        };
        Fmt(bf, "x/$", args);
    }
    if(flags & DEBUG){
        Buff_AddBytes(bf, (byte *)" ", 1);
        byte *b = (byte *)((void *)sl)+sizeof(RangeType);
        Bytes_Debug(bf, b, b+sl->type.range);
    }
    if(flags & (MORE|DEBUG)){
        Buff_AddBytes(bf, (byte *)">", 1);
    }

    return SUCCESS;
}

status Cstr_Print(Buff *bf, void *a, cls type, word flags){
    if(a == NULL){
        return SUCCESS;
    }

    Buff_AddBytes(bf, (byte *)a, strlen((char *)a));
    return SUCCESS;
}

status Str_Print(Buff *bf, void *a, cls type, word flags){
    void *args[5];
    Str *s = (Str*)as(a, TYPE_STR); 

    if((flags & (MORE|DEBUG)) == 0){
        return Buff_AddBytes(bf, s->bytes, s->length);
    }

    args[0] = Type_StateVec(bf->m, s->type.of, s->type.state);
    args[1] = I16_Wrapped(bf->m, s->length);
    args[2] = I16_Wrapped(bf->m, s->alloc);
    args[3] = NULL;

    if(flags & DEBUG){
        Fmt(bf, "Str<$ $/$:\"^D", args);
    }else{
        Fmt(bf, "\"^D", NULL); 
    }

    if(flags & DEBUG){
        Bytes_Debug(bf, s->bytes, s->bytes+(s->length-1));
        Fmt(bf, "^d.\">", NULL);
    }else{
        Buff_AddBytes(bf, s->bytes, s->length);
        Fmt(bf, "^d.\"", NULL);
    }

    return SUCCESS;
}

status StrVec_Print(Buff *bf, void *a, cls type, word flags){
    StrVec *vObj = (StrVec *)as(a, TYPE_STRVEC);
    if(flags & MORE){
        Buff_AddBytes(bf, (byte *)"[", 1); 
    }

    if(flags & DEBUG){
        void *args[] = {
            Type_StateVec(bf->m, vObj->type.of, vObj->type.state),
            I32_Wrapped(bf->m, vObj->p->nvalues),
            I64_Wrapped(bf->m, vObj->total),
            NULL
        };
        Fmt(bf, "StrVec<$ $/$ [", args); 
    }

    Iter it;
    Iter_Init(&it, vObj->p);
    while((Iter_Next(&it) & END) == 0){
        Str *s = (Str *)it.value;
        if(s != NULL){
            if(flags & DEBUG){
                void *args[] = {
                    I32_Wrapped(bf->m, it.idx),
                    NULL
                };
                Fmt(bf, "$: ", args); 
                Str_Print(bf, s, type, flags|DEBUG);
            }else if((flags & MORE) && s->type.state){
                void *args[] = {s, NULL};
                if(s->type.state & NOOP){
                    Fmt(bf, "^I.$^i.", args); 
                }else{
                    Fmt(bf, "^D.$^d.", args); 
                }
            }else{
                Buff_AddBytes(bf, s->bytes, s->length);
            }

            if(flags & (MORE|DEBUG)){
                if((it.type.state & LAST) == 0){
                    Buff_AddBytes(bf, (byte *)",", 1);
                }
            }

        }
    }
    if(flags & DEBUG){
        Fmt(bf, "]>", NULL);
    }
    if(flags & MORE){
        Buff_AddBytes(bf, (byte *)"]",1);
    }
    return SUCCESS;
}

status Histo_Print(Buff *bf, void *a, cls type, word flags){
    Histo *hst = (Histo *)as(a, TYPE_HISTO);
    MemCh *m = bf->m;
    void *args[] = {
        Type_StateVec(m, hst->type.of, hst->type.state),
        I64_Wrapped(m, hst->total),
        I64_Wrapped(m, (i64)(hst->ratio * 100)),
        I64_Wrapped(m, hst->alpha),
        I64_Wrapped(m, hst->num),
        I64_Wrapped(m, hst->whitespace),
        I64_Wrapped(m, hst->punctuation),
        I64_Wrapped(m, hst->control),
        I64_Wrapped(m, hst->upper),
        NULL
    };
    return Fmt(bf, "Histo<@ $ $% /a$,n$,w$,p$,c$,u$>", args);
}

status Cursor_Print(Buff *bf, void *a, cls type, word flags){
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
    Abstract *focus = (Abstract *)curs->v;
    if((curs->type.state & PROCESSING) && curs->ptr != NULL){
        endPos = pos+length;
        focus = (Abstract *)Str_Ref(bf->m,
            curs->ptr,
            curs->end - curs->ptr + 1,
            curs->end - curs->ptr + 1,
            STRING_CONST|DEBUG);
    }

    if(flags & DEBUG){
        void *args[] = {
            Type_StateVec(bf->m, curs->type.of, curs->type.state),
            I64_Wrapped(bf->m, pos),
            I64_Wrapped(bf->m, endPos),
            I64_Wrapped(bf->m, length),
            I64_Wrapped(bf->m, curs->v->total),
            focus,
            NULL
        };
        return  Fmt(bf, "Curs<$ $..$ $of$ ^d.: \"^D.$^d.\">", args); 
    }else{
        i64 length = (i64)(curs->end - curs->ptr)+1;

        void *args[] = {
            Type_StateVec(bf->m, curs->type.of, curs->type.state),
            I64_Wrapped(bf->m, pos),
            I64_Wrapped(bf->m, curs->v->total),
            ((pos >= 8) ? Str_CstrRef(bf->m, "...") : Str_CstrRef(bf->m, "")),
            focus,
            ((endPos < curs->v->total) ? Str_CstrRef(bf->m, "...") 
                : Str_CstrRef(bf->m, "")),
            NULL
        };

        return  Fmt(bf, "Curs<$ $/$ ^D.\"$^EI.$^ei.$\"^id.>", args);
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

    if(histoLabels == NULL){
        histoLabels = (Str **)Arr_Make(m, 17);
        histoLabels[9] = Str_CstrRef(m, "ENFORCE");
        histoLabels[10] = Str_CstrRef(m, "CODE");
        histoLabels[11] = Str_CstrRef(m, "CONTROL");
        histoLabels[12] = Str_CstrRef(m, "UNICODE");
        Lookup_Add(m, lk, TYPE_HISTO, (void *)histoLabels);
        r |= SUCCESS;
    }

    return r;
}

boolean Str_Empty(void *_a){
    Str *s = (Str *)_a;
    return s->length == 0;
}

boolean StrVec_Empty(void *_a){
    StrVec *v = (StrVec *)_a;
    return v->total == 0;
}

status Str_ToSInit(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_STR, (void *)Str_Print);
    r |= Lookup_Add(m, lk, TYPE_CSTR, (void *)Cstr_Print);
    r |= Lookup_Add(m, lk, TYPE_STRVEC, (void *)StrVec_Print);
    r |= Lookup_Add(m, lk, TYPE_CURSOR, (void *)Cursor_Print);
    r |= Lookup_Add(m, lk, TYPE_BYTES_POINTER, (void *)BytesLit_Print);
    r |= Lookup_Add(m, lk, TYPE_HISTO, (void *)Histo_Print);
    r |= Str_InitLabels(m, ToSFlagLookup);

    r |= Lookup_Add(m, EmptyLookup, TYPE_STR, (void *)Str_Empty);
    r |= Lookup_Add(m, EmptyLookup, TYPE_STRVEC, (void *)StrVec_Empty);

    return r;
}
