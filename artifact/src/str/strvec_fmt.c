#include <external.h>
#include <caneka.h>

static i64 StrVec_FmtAddArgs(MemCtx *m, StrVec *v, char *fmt, va_list args){
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
            if(c == 'v'){
                s = Str_FromAbs(m, va_arg(args, Abstract *));
                StrVec_Add(v, s);
                total += s->length;
                state = SUCCESS; 
                goto next;
            }else if(c == 'd'){
                total += Str_Debug(m, v, va_arg(args, Abstract *), 0, FALSE);
                state = SUCCESS; 
                goto next;
            }else if(c == 'D'){
                total += Str_Debug(m, v, va_arg(args, Abstract *), 0, TRUE);
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
                    i = (i64)va_arg(args, i32);
                }else{ /* 4 is the default */
                    i = (i64)va_arg(args, i32);
                    ptr--;
                }
                s = Str_FromI64(m, i);
                StrVec_Add(v, s);
                total += s->length;
                state = SUCCESS; 
                goto next;
            }else if(c == 'a'){
                i32 l = 2;
                s = Str_Ref(m, (byte *)"*=", l, l);
                StrVec_Add(v, s);
                total += s->length;
                util u = (i64)va_arg(args, util);
                s = Str_FromI64(m, u);
                StrVec_Add(v, s);
                StrVec_Add(v, s);
                total += s->length;
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
                StrVec_Add(v, s);
                total += s->length;
                state = SUCCESS; 
                goto next;
            }
        }else if(state == CONTINUED){
            Str *s = Str_FromAnsi(m, &ptr, end);
            StrVec_Add(v, s);
            total += s->length;
            state = SUCCESS; 
            goto next;
        }
        
        c = *ptr;
        if(c == '^'){
           state = CONTINUED; 
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
                StrVec_Add(v, s);
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
            StrVec_Add(v, s);
            total += s->length;
        }
   }

    return total; 
}

i64 StrVec_FmtAdd(MemCtx *m, StrVec *v, char *fmt, ...){
	va_list args;
    va_start(args, fmt);
    return StrVec_FmtAddArgs(m, v, fmt, args);
}

StrVec *StrVec_Fmt(MemCtx *m, char *fmt, ...){
    StrVec *v = StrVec_Make(m);
	va_list args;
    va_start(args, fmt);
    StrVec_FmtAddArgs(m, v, fmt, args);
    return v;
}

