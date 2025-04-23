#include <external.h>
#include <caneka.h>

i64 StrVec_FmtHandle(Stream *sm, char *fmt, void **args){
    size_t l = strlen(fmt);
    MemCh *m = sm->m;
    char *end = fmt+l;
    char *ptr = fmt;
    Str *s = NULL;
    StrVec *v = NULL;
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
                Abstract *a = (Abstract *)*(args++);
                if(a != NULL || a->type.of == TYPE_STR){
                    s = (Str *)a;
                    Stream_To(sm, s->bytes, s->length);
                }else{
                    v = StrVec_FromAbs(m, a);
                    Stream_VecTo(sm, v);
                }
                total += s->length;
                state = SUCCESS; 
                goto next;
            }else if(c == 'T'){
                Abstract *a = (Abstract *)*(args++);
                cls type = (cls)((i64 *)*(args++));
                total += Str_Debug(sm, a, type, FALSE);
                state = SUCCESS; 
                goto next;
            }else if(c == 's'){
                Abstract *a = (Abstract *)*(args++);
                if(a == NULL){
                    total += Stream_To(sm, (byte *)"NULL", 4);
                }else if(a->type.of == TYPE_STR){
                    s = (Str *)a;
                    if(sm->type.state & STREAM_STRVEC){
                        Cursor_Add(sm->dest.curs, s);
                        total += s->length;
                    }else{
                        total += Stream_To(sm, s->bytes, s->length);
                    }
                }else{
                    as(a, TYPE_STR);
                }
                state = SUCCESS; 
                goto next;
            }else if(c == 'o'){
                Abstract *a = (Abstract *)*(args++);
                s = Str_CstrRef(m, (char *)Type_ToChars(a->type.of));
                Stream_To(sm, s->bytes, s->length);
                state = SUCCESS; 
                goto next;
            }else if(c == 'O'){
                i32 i = (i32)*((i32 *)*(args++));
                s = Str_CstrRef(m, (char *)Type_ToChars((cls)i));
                Stream_To(sm, s->bytes, s->length);
                state = SUCCESS; 
                goto next;
            }else if(c == 'd'){
                Abstract *a = (Abstract *)*(args++);
                total += Str_Debug(sm, a, 0, FALSE);
                state = SUCCESS; 
                goto next;
            }else if(c == 'D'){
                Abstract *a = (Abstract *)*(args++);
                total += Str_Debug(sm, a, 0, TRUE);
                state = SUCCESS; 
                goto next;
            }else if(c == 'c'){
                char *cstr = (char *)*(args++);
                s = Str_CstrRef(m, cstr);
                Stream_To(sm, s->bytes, s->length);
                state = SUCCESS; 
                goto next;
            }else if(c == 'C'){
                char *cstr = (char *)*(args++);
                size_t length = (size_t)*(args++);
                Stream_To(sm, cstr, length);
                state = SUCCESS; 
                goto next;
            }else if(c == 'i'){
                c = *(++ptr);
                i64 *ip = *(args++);
                i64 i = 0;
                if(c == '1'){
                    i = (i64)*((i8 *)ip);
                }else if(c == '2'){
                    i = (i64)*((i16 *)ip);
                }else if(c == '4'){
                    i = (i64)*((i32 *)ip);
                }else if(c == '8'){
                    i = (i64)*((i64 *)ip);
                }else{ /* 4 is the default */
                    i = (i64)*((i32 *)ip);
                    ptr--;
                }
                s = Str_FromI64(m, i);
                Stream_To(sm, s->bytes, s->length);
                total += s->length;
                state = SUCCESS; 
                goto next;
            }else if(c == 'a'){
                i32 l = 2;
                s = Str_Ref(m, (byte *)"*", l, l);
                Stream_To(sm, s->bytes, s->length);
                total += s->length;
                util u = (util)*(args++);
                s = Str_FromI64(m, u);
                Stream_To(sm, s->bytes, s->length);
                total += s->length;
                state = SUCCESS; 
                goto next;
            }else if(c == 'b'){
                void *b = *(args++);
                c = *(++ptr);
                i64 sz = 1;
                if(c == '1'){
                    sz = 1;
                }else if(c == '2'){
                    sz = 2;
                }else if(c == '4'){
                    sz = 4;
                }else if(c == '8'){ 
                    sz = 8;
                }else{ 
                    ptr--;
                }
                total += Bits_Print(sm, b, sz, FALSE);
                state = SUCCESS; 
                goto next;
            }else if(c == 'B'){
                void *b = *(args++);
                size_t sz = *((size_t *)(args++));
                total += Bits_Print(sm, b, sz, TRUE);
                state = SUCCESS; 
                goto next;
            }else if(c == '+'){
                StrVec *v2 = (StrVec *)*(args++);
                total += Stream_VecTo(sm, v2);
                state = SUCCESS; 
                goto next;
            }else if(c == 'u'){
                c = *(++ptr);
                i64 i = 0;
                if(c == '2'){
                    i = (i64)*((i64 *)(args++));
                }else if(c == '1'){
                    i = (i64)*((i64 *)(args++));
                }else if(c == '8'){
                    i = (i64)*((i64 *)(args++));
                }else if(c == '4'){ 
                    i = (i64)*(args++);
                }else{ /* 4 is the default */
                    i = (i64)*((i64 *)(args++));
                    ptr--;
                }
                s = Str_FromI64(m, i);
                Stream_To(sm, s->bytes, s->length);
                total += s->length;
                state = SUCCESS; 
                goto next;
            }else if(c == '^'){
                char *cstr = (char *)*(args++);
                char *cstr_end = cstr+(strlen(cstr)-1);
                Str *s = Str_FromAnsi(m, &cstr, cstr_end);
                Stream_To(sm, s->bytes, s->length);
                total += s->length;
                state = SUCCESS; 
                goto next;
            }
        }else if(state == MORE){
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
           state = MORE; 
           goto outnext;
        }else if(c == '\\'){
           state = NOOP; 
           goto outnext;
        }else if(c == '_'){
           if(args == NULL){
                Fatal(0, FUNCNAME, FILENAME, LINENUMBER,
                    "Expecting arg, found NULL instead", NULL);
                return total;
           }
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

i64 StrVec_Fmt(Stream *sm, char *fmt, void **args){
    return StrVec_FmtHandle(sm, fmt, args);
}
