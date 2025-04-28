#include <external.h>
#include <caneka.h>

i64 Fmt(Stream *sm, char *fmt, Abstract *args[]){
    MemCh *m = sm->m;
    char *ptr = fmt;
    char *end = fmt+strlen(fmt);
    char *start = NULL;
    status state = SUCCESS;
    i64 total = 0;
    printf("\x1b[1m>> FMT:%s\x1b[22m\n", fmt);
    fflush(stdout);
    while(ptr < end){
        char c = *ptr;
        printf("c:%c remaining:%ld\n", c, end - ptr);
        fflush(stdout);
        if(state == SUCCESS){
            start = ptr;
            state = READY;
        }else if(state & PROCESSING){
            printf("  proc\n");
            fflush(stdout);
            Abstract *a = *(args++);
            if(a->type.of == TYPE_STR){
                Str *s = (Str *)a;
                if(s->type.state & STRING_FMT_ANSI){
                    char *cstr_end = (char *)s->bytes+(s->length-1);
                    s = Str_FromAnsi(m, (char **)&s->bytes, cstr_end);
                    total += Stream_Bytes(sm, s->bytes, s->length);
                    state = SUCCESS; 
                    goto next;
                }else if(s->type.state & STRING_BINARY){
                    total += Bits_Print(sm, s->bytes, s->length, 
                        ((s->type.state|sm->type.state) & DEBUG));
                    state = SUCCESS; 
                    goto next;
                }else if((state & MORE) && (sm->type.state & STREAM_STRVEC)){
                    StrVec_Add(sm->dest.curs->v, s);
                    state = SUCCESS; 
                    goto next;
                }
            }
            total += ToS(sm, a, a->type.of, 
                ((sm->type.state & DEBUG)|(state & MORE)));
            state = SUCCESS; 
            goto next;
        }else if(state == CONTINUE){
            printf("Calling Ansi %c of %s/%ld\n", *ptr, fmt, ptr-fmt);
            fflush(stdout);
            Str *s = Str_FromAnsi(m, &ptr, end);
            Stream_Bytes(sm, s->bytes, s->length);
            total += s->length;
            state = SUCCESS; 
            goto next;
        }
        
        c = *ptr;
        printf("  below %c\n", c);
        fflush(stdout);
        if(state == NOOP){
            printf("   noop");
            fflush(stdout);
            state = READY;
        }else if(c == '^'){
            printf("   ansi/CONTINUE\n");
            fflush(stdout);
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
                Stream_Bytes(sm, (byte *)start, length);
                total += length;
           }
       }
       ptr++;
       continue;
    }

    if(state != SUCCESS && start != ptr){
        word length = (word)(ptr - start);
        if(length > 0){
            Stream_Bytes(sm, (byte *)start, length);
            total += length;
        }
    }

    return total; 
}
