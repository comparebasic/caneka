#include <external.h>
#include <caneka.h>

i64 Fmt(Stream *sm, char *fmt, Abstract *args[]){
    MemCh *m = sm->m;
    char *ptr = fmt;
    char *end = fmt+strlen(fmt);
    char *start = NULL;
    status state = SUCCESS;
    i64 total = 0;
    while(ptr != end){
        char c = *ptr;
        if(state == SUCCESS){
            start = ptr;
            state = READY;
        }else if(state & PROCESSING){
            Abstract *a = *(args++);
            if(a->type.of == TYPE_STR){
                Str *s = (Str *)a;
                if(s->type.state & STRING_FMT_ANSI){
                    char *cstr_end = (char *)s->bytes+(s->length-1);
                    s = Str_FromAnsi(m, (char **)&s->bytes, cstr_end);
                    total += Stream_To(sm, s->bytes, s->length);
                    state = SUCCESS; 
                    goto next;
                }else if(s->type.state & STRING_BINARY){
                    total += Bits_Print(sm, s->bytes, s->length, 
                        ((s->type|sm->type.state) & DEBUG));
                    state = SUCCESS; 
                    goto next;
                }
            }
            total += ToStream(sm, a, type, 
                ((sm->type.state & DEBUG)|(state & MORE)));
            state = SUCCESS; 
            goto next;
        }else if(state == CONTINUE){
            Str *s = Str_FromAnsi(m, &ptr, end);
            Stream_To(sm, s->bytes, s->length);
            total += s->length;
            state = SUCCESS; 
            goto next;
        }
        
        c = *ptr;
        if(state == NOOP){
            state = READY;
        }else if(c == '^'){
           state = CONTINUE; 
           goto outnext;
        }else if(c == '\\'){
           state = NOOP; 
           goto outnext;
        }else if(c == '$' || c == '@'){
            if(args == NULL){
                Abstract *args[] = {
                    (Abstract *)Str_CstrRef(m, fmt),
                    NULL
                };
                Fatal(FUNCNAME, FILENAME, LINENUMBER,
                    "Expecting arg, found NULL instead, '$'", args);
                return total;
            }
            state = PROCESSING; 
            if(c == '@'){
                state |= MORE;
            }
            goto outnext;
        }
next:
        ptr++;
        continue;
outnext:
       if(start != ptr){
            word length = (word)(ptr - start);
            if(length > 0){
                s = Str_Ref(m, (byte *)start, length, length);
                Stream_To(sm, s->bytes, s->length);
                total += s->length;
            }
       }
       ptr++;
       continue;
    }

    if(state != SUCCESS && start != ptr){
        word length = (word)(ptr - start);
        if(length > 0){
            s = Str_Ref(m, (byte *)start, length, length);
            Stream_To(sm, s->bytes, s->length);
            total += s->length;
        }
    }

    return total; 
}
