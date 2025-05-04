#include <external.h>
#include <caneka.h>

i64 Fmt(Stream *sm, char *fmt, Abstract *args[]){
    MemCh *m = sm->m;
    char *ptr = fmt;
    char *end = fmt+(strlen(fmt)-1);
    char *start = fmt;
    status state = SUCCESS;
    i64 total = 0;
    while(ptr <= end){
        char c = *ptr;
        if((state & NOOP) != 0){
            state &= ~NOOP;
            goto next;
        }else if(c == '\\'){
            state |= NOOP;
            goto next;
        }else if(c == '$' || c == '@'){
            if(c == '@'){
                state |= MORE;
            }else{
                state &= ~MORE;
            }

            if(args == NULL){
                Abstract *args[] = {
                    (Abstract *)Str_CstrRef(m, fmt),
                    NULL
                };
                Fatal(FUNCNAME, FILENAME, LINENUMBER,
                    "Expecting arg, found NULL instead, '$'", args);
                return total;
            }

            if(start != ptr){
               word length = (word)(ptr - start);
                Stream_Bytes(sm, (byte *)start, length);
                total += length;
            }
            start = ptr+1;

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
                    total += Stream_Bytes(sm, s->bytes, s->length);
                    goto next;
                }else if(s->type.state & STRING_BINARY){
                    total += Bits_Print(sm, s->bytes, s->length, 
                        ((s->type.state|sm->type.state) & DEBUG));
                    goto next;
                }
            }else if(a->type.of == TYPE_WRAPPED_PTR && ((Single *)a)->objType.of != 0){
                Single *sg = (Single *)a;
                type = sg->objType.of;
                a = sg->val.ptr;
            }
            total += ToS(sm, a, type, 
                ((sm->type.state & DEBUG)|(state & MORE)) | a->type.state);
            state = SUCCESS;
            goto next;
        }else if(c == '^'){
            if(start != ptr){
               word length = (word)(ptr - start);
               if(length > 0){
                    Stream_Bytes(sm, (byte *)start, length);
                    total += length;
               }
            }

            ptr++;
            Str *s = Str_FromAnsi(m, &ptr, end);
            Stream_Bytes(sm, s->bytes, s->length);
            total += s->length;
            start = ptr+1;
        }else{
next:
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

    return total; 
}

FmtLine *FmtLine_FromSpan(MemCh *m, char *fmt, Span *p){
    FmtLine *ln = (FmtLine *)MemCh_Alloc(m, sizeof(FmtLine));
    ln->type.of = TYPE_FMT_LINE;
    ln->fmt = fmt;
    ln->args = Span_ToArr(m, p);
    return ln;
}

FmtLine *FmtLine_Make(MemCh *m, char *fmt, Abstract **args){
    FmtLine *ln = (FmtLine *)MemCh_Alloc(m, sizeof(FmtLine));
    ln->type.of = TYPE_FMT_LINE;
    ln->fmt = fmt;
    ln->args = args;
    return ln;
}
