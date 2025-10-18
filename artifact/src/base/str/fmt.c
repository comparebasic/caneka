#include <external.h>
#include <caneka.h>

i64 Fmt(Stream *sm, char *fmt, Abstract *args[]){
    MemCh *m = sm->m;
    char *ptr = fmt;
    char *end = fmt+(strlen(fmt)-1);
    char *start = fmt;
    status state = SUCCESS;
    i64 total = 0;
    if((sm->type.state & STREAM_STRVEC) == 0){
        sm->m->type.range++;
    }
    while(ptr <= end){
        char c = *ptr;
        if((state & NOOP) != 0){
            state &= ~NOOP;
            goto next;
        }else if(c == '\\' || c == '$' || c == '@' || c == '&' || c == '^'){
            if(c == '\\'){
                if(start < ptr){
                    word length = (word)((ptr) - start);
                    Stream_Bytes(sm, (byte *)start, length);
                    total += length;
                }
                state |= NOOP;
                start = ptr+1;
                goto next;
            }else{
                if(start != ptr){
                    word length = (word)(ptr - start);
                    Stream_Bytes(sm, (byte *)start, length);
                    total += length;
                }
                start = ptr+1;
            }
        }

        if(c == '$' || c == '@' || c == '&'){
            if(c == '@'){
                state |= MORE;
            }else{
                state &= ~MORE;
            }
            if(c == '&'){
                state |= (DEBUG|MORE);
            }else{
                state &= ~DEBUG;
            }

            if(args == NULL){
                Abstract *args[] = {
                    (Abstract *)Str_CstrRef(m, fmt),
                    NULL
                };
                Error(m, FUNCNAME, FILENAME, LINENUMBER,
                    "Expecting arg, found NULL instead, '$'", args);
                return total;
            }

            Abstract *a = *(args++);
            if(a == NULL){
                if(state & (DEBUG|MORE)){
                    total += Stream_Bytes(sm, (byte *)"NULL", 4);
                }
                goto next;
            }
            cls type = a->type.of;
            if(a->type.of == TYPE_STR){
                Str *s = (Str *)a;
                if(s->type.state & STRING_FMT_ANSI){
                    char *cstr_end = (char *)s->bytes+(s->length-1);
                    s = Str_FromAnsi(m, (char **)&s->bytes, cstr_end);
                    if((state & (DEBUG|MORE)) != (DEBUG|MORE)){
                        total += Stream_Bytes(sm, s->bytes, s->length);
                        goto next;
                    }else{
                        a = (Abstract *)a;
                        type = s->type.of;
                    }
                }else if(s->type.state & STRING_BINARY){
                    total += Bits_Print(sm, s->bytes, s->length, 
                        ((s->type.state|sm->type.state) & DEBUG));
                    goto next;
                }
            }else if(a->type.of == TYPE_STREAM_TASK){
                StreamTask *tsk = (StreamTask *)a;
                total += tsk->func(sm, tsk->a); 
                state |= SUCCESS;
                goto next;
            }else if(a->type.of == TYPE_ARRAY){
                StreamTask *tsk = (StreamTask *)a;
                total += tsk->func(sm, tsk->a); 
                state |= SUCCESS;
                goto next;
            }else if(a->type.of == TYPE_WRAPPED_PTR && ((Single *)a)->objType.of != 0){
                Single *sg = (Single *)a;
                type = sg->objType.of;
                a = sg->val.ptr;
            }
            total += ToS(sm, a, type, (state & (MORE|DEBUG)));
            state |= SUCCESS;
            goto next;
        }else if(c == '^'){
            ptr++;
            Str *s = Str_ConsumeAnsi(m, &ptr, end, TRUE);
            Stream_Bytes(sm, s->bytes, s->length);
            total += s->length;
            start = ptr+1;
            if((sm->type.state & STREAM_STRVEC) == 0){
                MemCh_Free(sm->m);
            }
        }else{
next:
            if((sm->type.state & STREAM_STRVEC) == 0){
                MemCh_Free(sm->m);
            }
            ptr++;
        }
    }

    if(start < ptr){
        word length = (word)(ptr - start);
        if(length > 0){
            Stream_Bytes(sm, (byte *)start, length);
            total += length;
        }
    }

    if((sm->type.state & STREAM_STRVEC) == 0){
        sm->m->type.range--;
    }

    return total; 
}

FmtLine *FmtLine_FromSpan(MemCh *m, char *fmt, Span *p){
    FmtLine *ln = (FmtLine *)MemCh_Alloc(m, sizeof(FmtLine));
    ln->type.of = TYPE_FMT_LINE;
    ln->fmt = fmt;
    ln->args = Span_ToArr(m, p);
    return ln;
}

StrVec *Fmt_ToStrVec(MemCh *m, char *fmt, Abstract **args){
    Stream *sm = Stream_MakeStrVec(m); 
    Fmt(sm, fmt, args); 
    return sm->dest.curs->v;
}

FmtLine *FmtLine_Make(MemCh *m, char *fmt, Abstract **args){
    FmtLine *ln = (FmtLine *)MemCh_Alloc(m, sizeof(FmtLine));
    ln->type.of = TYPE_FMT_LINE;
    ln->fmt = fmt;
    ln->args = args;
    return ln;
}
