#include <external.h>
#include <caneka.h>

Abstract *FmtVar_Get(MemCh *m, Str *key, Abstract *arg){
    StrVec *path = StrVec_From(m, key);
    Path_DotAnnotate(m, path);
    Abstract *a = NULL;
    Str *k = Span_Get(path->p, 0);
    if(Equals((Abstract *)k, (Abstract *)Str_FromCstr(m, "STACK", ZERO))){
        a = (Abstract *)DebugStack_Get();
        if(path->p->nvalues > 1){
            k = Span_Get(path->p, 2);
            StackEntry *entry = (StackEntry *)as(a, TYPE_DEBUG_STACK_ENTRY);
            if(Equals((Abstract *)k, (Abstract *)Str_FromCstr(m, "name", ZERO))){
                a = (Abstract *)Str_FromCstr(m, entry->funcName, STRING_COPY);
            }else if(Equals((Abstract *)k, (Abstract *)Str_FromCstr(m, "ref", ZERO))){
                a = (Abstract *)entry->ref;
            }else{
                a = NULL;
            }
        }
    }else if(Equals((Abstract *)k, (Abstract *)Str_FromCstr(m, "TIME", ZERO))){
        if(path->p->nvalues > 1){
            k = Span_Get(path->p, 2);
            if(Equals((Abstract *)k, (Abstract *)Str_FromCstr(m, "human", ZERO))){
                a = (Abstract *)MicroTime_ToStr(m, MicroTime_Now());  
            }else{
                a = NULL;
            }
        }else{
            a = (Abstract *)Str_FromI64(m, MicroTime_Now());  
        }
    }else if(arg != NULL && arg->type.of == TYPE_TABLE){
        return Table_Get((Table *)arg, (Abstract *)key);
    }
    return a;
}

status Fmt(Buff *bf, char *fmt, Abstract *args[]){
    MemCh *m = bf->m;
    char *ptr = fmt;
    char *end = fmt+(strlen(fmt)-1);
    char *start = fmt;
    status state = SUCCESS;
    while(ptr <= end){
        char c = *ptr;
        if((state & NOOP) != 0){
            state &= ~NOOP;
            goto next;
        }else if(c == '\\' || c == '$' || c == '@' || c == '&' || c == '^'){
            if(c == '\\'){
                if(start < ptr){
                    word length = (word)((ptr) - start);
                    Buff_AddBytes(bf, (byte *)start, length);
                }
                state |= NOOP;
                start = ptr+1;
                goto next;
            }else{
                if(start != ptr){
                    word length = (word)(ptr - start);
                    Buff_AddBytes(bf, (byte *)start, length);
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
                return ERROR;
            }

            Abstract *a = *(args++);
            if(a == NULL){
                if(state & (DEBUG|MORE)){
                    Buff_AddBytes(bf, (byte *)"NULL", 4);
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
                        Buff_AddBytes(bf, s->bytes, s->length);
                        goto next;
                    }else{
                        a = (Abstract *)a;
                        type = s->type.of;
                    }
                }else if(s->type.state & STRING_BINARY){
                    Bits_Print(bf, s->bytes, s->length, 
                        ((s->type.state|bf->type.state) & DEBUG));
                    goto next;
                }
            }else if(a->type.of == TYPE_ARRAY){
                state |= SUCCESS;
                goto next;
            }else if(a->type.of == TYPE_WRAPPED_PTR && ((Single *)a)->objType.of != 0){
                Single *sg = (Single *)a;
                type = sg->objType.of;
                a = sg->val.ptr;
            }


            ToS(bf, a, type, (state & (MORE|DEBUG)));
            state |= SUCCESS;
            goto next;
        }else if(c == '^'){
            ptr++;
            if(*ptr == '{' && ptr < end){
                char c = *(ptr+1);
                word flags = ZERO;
                if(c == '@'){
                    flags |= MORE;
                    ptr++;
                }else if(c == '&'){
                    flags |= (MORE|DEBUG);
                    ptr++;
                }
                ptr++;
                char *keyStart = ptr;
                word keyLen = 0;
                while(*ptr != '}' && ptr <= end){ keyLen++; ptr++; };
                Str *key = Str_Ref(bf->m, (byte *)keyStart, keyLen, keyLen, STRING_CONST);
                Abstract *var = FmtVar_Get(m, key, *args);
                if(var != NULL){
                    ToS(bf, var, 0, flags);
                }else{
                    Buff_AddBytes(bf, (byte *)"NULL", 4);
                }
                args++;
            }else{
                Str *s = Str_ConsumeAnsi(m, &ptr, end, TRUE);
                Buff_AddBytes(bf, s->bytes, s->length);
            }
            start = ptr+1;
        }else{
next:
            ptr++;
        }
    }

    if(start < ptr){
        word length = (word)(ptr - start);
        if(length > 0){
            Buff_AddBytes(bf, (byte *)start, length);
        }
    }

    return SUCCESS; 
}

FmtLine *FmtLine_FromSpan(MemCh *m, char *fmt, Span *p){
    FmtLine *ln = (FmtLine *)MemCh_Alloc(m, sizeof(FmtLine));
    ln->type.of = TYPE_FMT_LINE;
    ln->fmt = fmt;
    ln->args = Span_ToArr(m, p);
    return ln;
}

StrVec *Fmt_ToStrVec(MemCh *m, char *fmt, Abstract **args){
    Buff *bf = Buff_Make(m, ZERO);
    Fmt(bf, fmt, args); 
    return bf->v;
}

FmtLine *FmtLine_Make(MemCh *m, char *fmt, Abstract **args){
    FmtLine *ln = (FmtLine *)MemCh_Alloc(m, sizeof(FmtLine));
    ln->type.of = TYPE_FMT_LINE;
    ln->fmt = fmt;
    ln->args = args;
    return ln;
}
