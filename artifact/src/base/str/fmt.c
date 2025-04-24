#include <external.h>
#include <caneka.h>

i64 Fmt(Stream *sm, char *fmt, Single args[]){
    size_t l = strlen(fmt);
    MemCh *m = sm->m;
    char *end = fmt+l;
    char *ptr = fmt;
    Str *s = NULL;
    StrVec *v = NULL;
    char *start = NULL;
    status state = SUCCESS;
    i64 total = 0;
    Single *sg = args;
    while(ptr != end){
        char c = *ptr;
        if(state == SUCCESS){
            start = ptr;
            state = READY;
        }else if(state == PROCESSING){
            if(sg->type.of == TYPE_WRAPPED_CSTR){
                char *cstr = (char *)(sg++)->type.ptr;
                if(sg->type.state & FMT_TYPE_EXTRA_ARG){
                    sg++;
                    if(sg->type.of = TYPE_WRAPPED_I64){
                        len = (i64)sg->val.util;
                    }
                }else{
                    len = strlen(cstr);
                }

                if(sg->type.state & FMT_TYPE_ANSI){
                    char *cstr_end = cstr+(strlen(cstr)-1);
                    Str *s = Str_FromAnsi(m, &cstr, cstr_end);
                    Stream_To(sm, s->bytes, s->length);
                    total += s->length;
                }else{
                    total += Stream_To(sm, cstr, len);
                }
                state = SUCCESS; 
                goto next;
            }else{
                Abstract *a = (Abstract *)(sg++);
                if(((sm->type.debug & DEBUG) | 
                        (v->type.state & DEBUG)) != 0){
                    cls type = 0;
                    if(a->type.state & FMT_TYPE_EXTRA_ARG){
                        if(sg->type.of == TYPE_WRAPPED_I16){
                            type = sg->val.w;
                            sg++;
                        }
                    }
                    total += Str_Debug(sm, a, type, TRUE);
                }else if(a != NULL || a->type.of == TYPE_STR){
                    s = (Str *)a;
                    total += Stream_To(sm, s->bytes, s->length);
                }else{
                    v = StrVec_FromAbs(m, a);
                    total += Stream_VecTo(sm, v);
                }
                total += s->length;
                state = SUCCESS; 
                goto next;
            }else if(Ifc_Match(sg, TYPE_WRAPPED) && 
                    (sg->type.state & FMT_TYPE_BITS)){
                size_t sz = 0;
                void *ptr = NULL;
                boolean verbose = (sg->type.state & FMT_TYPE_VERBOSE) != 0;
                if(sg->type.of == TYPE_WRAPPED_PTR){
                    if(sg->type.state & FMT_TYPE_EXTRA_ARG){
                        sg++;
                        if(sg->type.of == TYPE_WRAPPED_I64){
                            sz = sg->val.value;
                        }
                    }else{
                        Fatal(FUNCNAME, FILENAME, LINENUMBER,
                            "Expected extra arg", NULL);
                    }
                    ptr = sg->val.ptr;
                }else if(sg->type.of == TYPE_WRAPPED_I64){
                    sz = 8;
                    ptr = &sg->val.value;
                }else if(sg->type.of == TYPE_WRAPPED_I32){
                    sz = 4;
                    ptr = &sg->val.i;
                }else if(sg->type.of == TYPE_WRAPPED_I16){
                    sz = 2;
                    ptr = &sg->val.w;
                }else if(sg->type.of == TYPE_WRAPPED_I8){
                    sz = 1;
                    ptr = &sg->val.b;
                }

                total += Bits_Print(sm, ptr, sz, verbose);
                state = SUCCESS; 
                goto next;
            }else if(sg->type.of == TYPE_WRAPPED_I64){
                s = Str_FromI64(m, sg->val.value);
                Stream_To(sm, s->bytes, s->length);
                total += s->length;
                state = SUCCESS; 
                goto next;
            }else if(sg->type.of == TYPE_WRAPPED_I32){
                s = Str_FromI64(m, sg->val.i);
                Stream_To(sm, s->bytes, s->length);
                total += s->length;
                state = SUCCESS; 
                goto next;
            }else if(sg->type.of == TYPE_WRAPPED_I16){
                s = Str_FromI64(m, sg->val.w);
                Stream_To(sm, s->bytes, s->length);
                total += s->length;
                state = SUCCESS; 
                goto next;
            }else if(sg->type.of == TYPE_WRAPPED_I8){
                s = Str_FromI64(m, sg->val.b);
                Stream_To(sm, s->bytes, s->length);
                total += s->length;
                state = SUCCESS; 
                goto next;
            }else if(sg->type.of == TYPE_WRAPPED_PTR){
                i32 l = 2;
                s = Str_Ref(m, (byte *)"*", l, l);
                Stream_To(sm, s->bytes, s->length);
                total += s->length;
                util u = (util)*(sg++);
                s = Str_FromI64(m, u);
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
        }else if(c == '$'){
           if(args == NULL){
                Single args[] = {
                    {{TYPE_WRAPPED_CSTR, 0}, .val.ptr = fmt}
                };
                Fatal(0, FUNCNAME, FILENAME, LINENUMBER,
                    "Expecting arg, found NULL instead, '$'", args);
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
