#include <external.h>
#include <caneka.h>

#include "inline/handle_io.c"

i64 StrVec_FmtHandle(MemCh *m, StrVec *v, char *fmt, va_list args, i32 fd){
    size_t l = strlen(fmt);
    char *end = fmt+l;
    char *ptr = fmt;
    Str *s = NULL;
    char *start = NULL;
    status state = SUCCESS;
    i64 total = 0;
    while(ptr != end){
        char c = *ptr;
        if(state == SUCCESS){
            start = ptr;
            state = READY;
        }else if(state == PROCESSING){
            if(c == 't'){
                s = Str_FromAbs(m, va_arg(args, Abstract *));
                handleIo(v, fd, s);
                total += s->length;
                state = SUCCESS; 
                goto next;
            }else if(c == 'T'){
                Abstract *a = va_arg(args, Abstract *);
                cls type = va_arg(args, i32);
                total += Str_Debug(m, v, fd, a, (cls)type, FALSE);
                state = SUCCESS; 
                goto next;
            }else if(c == 'o'){
                Abstract *a = (Abstract *)va_arg(args, Abstract *);
                s = Str_CstrRef(m, (char *)Type_ToChars(a->type.of));
                handleIo(v, fd, s);
                state = SUCCESS; 
                goto next;
            }else if(c == 'O'){
                i32 i = (i32)va_arg(args, i32);
                s = Str_CstrRef(m, (char *)Type_ToChars((cls)i));
                handleIo(v, fd, s);
                state = SUCCESS; 
                goto next;
            }else if(c == 'd'){
                Abstract *a = va_arg(args, Abstract *);
                total += Str_Debug(m, v, fd, a, 0, FALSE);
                state = SUCCESS; 
                goto next;
            }else if(c == 'D'){
                Abstract *a = va_arg(args, Abstract *);
                total += Str_Debug(m, v, fd, a, 0, TRUE);
                state = SUCCESS; 
                goto next;
            }else if(c == 'c'){
                char *cstr = (char *)va_arg(args, char *);
                s = Str_CstrRef(m, cstr);
                handleIo(v, fd, s);
                state = SUCCESS; 
                goto next;
            }else if(c == 'i'){
                c = *(++ptr);
                i64 i = 0;
                /*
                if(c == '2'){
                    i = (i64)va_arg(args, word);
                }else if(c == '1'){
                    i = (i64)va_arg(args, byte);
                }else if(c == '8'){
                    */
                if(c == '8'){
                    i = (i64)va_arg(args, i64);
                }else if(c == '4'){ 
                    i = (i32)va_arg(args, i32);
                }else{ /* 4 is the default */
                    i = (i64)va_arg(args, i32);
                    ptr--;
                }
                s = Str_FromI64(m, (i64)i);
                handleIo(v, fd, s);
                total += s->length;
                state = SUCCESS; 
                goto next;
            }else if(c == 'a'){
                i32 l = 2;
                s = Str_Ref(m, (byte *)"*", l, l);
                handleIo(v, fd, s);
                total += s->length;
                util u = (i64)va_arg(args, util);
                s = Str_FromI64(m, u);
                handleIo(v, fd, s);
                total += s->length;
                state = SUCCESS; 
                goto next;
            }else if(c == '+'){
                StrVec *v2 = (StrVec *)va_arg(args, StrVec *);
                total += handleVecIo(v, fd, v2);
                state = SUCCESS; 
                goto next;
            }else if(c == 'u'){
                c = *(++ptr);
                i64 i = 0;
                /*
                if(c == '2'){
                    i = (i64)va_arg(args, word);
                }else if(c == '1'){
                    i = (i64)va_arg(args, byte);
                }else if(c == '8'){
                    */
                if(c == '8'){
                    i = (i64)va_arg(args, util);
                }else if(c == '4'){ 
                    i = (i64)va_arg(args, quad);
                }else{ /* 4 is the default */
                    i = (i64)va_arg(args, quad);
                    ptr--;
                }
                s = Str_FromI64(m, i);
                handleIo(v, fd, s);
                total += s->length;
                state = SUCCESS; 
                goto next;
            }else if(c == '^'){
                char *cstr = (char *)va_arg(args, char *);
                char *cstr_end = cstr+(strlen(cstr)-1);
                Str *s = Str_FromAnsi(m, &cstr, cstr_end);
                handleIo(v, fd, s);
                total += s->length;
                state = SUCCESS; 
                goto next;
            }
        }else if(state == MORE){
            Str *s = Str_FromAnsi(m, &ptr, end);
            handleIo(v, fd, s);
            total += s->length;
            state = SUCCESS; 
            goto next;
        }
        
        c = *ptr;
        if(c == '^'){
           state = MORE; 
           goto outnext;
        }else if(c == '_'){
           state = PROCESSING; 
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
                handleIo(v, fd, s);
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
            handleIo(v, fd, s);
            total += s->length;
        }
    }

    return total; 
}

i64 StrVec_FmtAdd(MemCh *m, StrVec *v, i32 fd, char *fmt, ...){
	va_list args;
    va_start(args, fmt);
    return StrVec_FmtHandle(m, v, fmt, args, fd);
}

StrVec *StrVec_Fmt(MemCh *m, char *fmt, ...){
    StrVec *v = StrVec_Make(m);
	va_list args;
    va_start(args, fmt);
    StrVec_FmtHandle(m, v, fmt, args, -1);
    return v;
}

