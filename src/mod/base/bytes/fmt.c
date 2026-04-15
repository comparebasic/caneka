/* Base.bytes.Fmt 
 *
 * String formatting output functions
 *
 * Uses *_tos files for several components to output a stirng format
 * for each Struct type.
 *
 * Used for debugging and string formatting.
 *
 */

#include <external.h>
#include "base_module.h"

void *FmtVar_Get(MemCh *m, Str *key, void *arg){
    /*  
     * :deprecated
     *
     * A place for global variable
     *
     * This will likely get replaced by a Lookup and moved out of Base into Ext
     * where it can take advantage of the Seel Object
     */
    StrVec *path = StrVec_From(m, key);
    Path_DotAnnotate(m, path);
    Abstract *a = NULL;
    Str *k = Span_Get(path->p, 0);
    if(Equals(k, K(m, "STACK"))){
        a = (Abstract *)DebugStack_Get();
        if(path->p->nvalues > 1){
            k = Span_Get(path->p, 2);
            StackEntry *entry = (StackEntry *)Ifc(m, a, TYPE_DEBUG_STACK_ENTRY);
            if(Equals(k, K(m, "name"))){
                a = (Abstract *)S(m, entry->funcName);
            }else if(Equals(k, K(m, "ref"))){
                a = (Abstract *)entry->ref;
            }else{
                a = NULL;
            }
        }
    }else if(Equals(k, K(m, "TIME"))){
        struct timespec now;
        Time_Now(&now);
        if(path->p->nvalues > 1){
            k = Span_Get(path->p, 2);
            if(Equals(k, K(m, "human"))){
                a = (Abstract *)Time_ToStr(m, &now);
            }else{
                a = NULL;
            }
        }else{
            StrVec *v = StrVec_Make(m);
            StrVec_Add(v, Str_FromI64(m, now.tv_sec));
            StrVec_Add(v, Str_Ref(m, (byte *)".", 1, 1, STRING_CONST|MORE));
            StrVec_Add(v, Str_FromI64(m, now.tv_nsec));
            a = (Abstract *)v;  
        }
    }else if(arg != NULL && ((Abstract *)arg)->type.of == TYPE_TABLE){
        return Table_Get((Table *)arg, key);
    }
    return a;
}

status Fmt(Buff *bf, char *fmt, void *args[]){
    /* 
     * Format a cstring with an array of void pointers
     *
     * Values are templated into strings in one of five ways
     *
     * $: Output the content version of the object if available.
     * throws an error if not available. For this like Str or StrVec or Array
     * this outputs the object without spaces or commas or debugging information 
     * kind.
     *
     * @: Output a minimal debugging version of the object
     *
     * &: Output a maximal debugging version of the object
     *
     * ^: Begin an Ansi color and style sequence. See Base.termio.AnsiStr for 
     * color and style details
     *
     * ^{TOKEN}: Output a global token with FmtVar_Get (deprecated)
     * 
     *
     */
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
                void *args[] = {
                    Str_CstrRef(m, fmt),
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
                        ((s->type.state|bf->type.state) & (MORE|DEBUG)));
                    goto next;
                }else if((s->type.state & DEBUG) && (state & DEBUG) == 0){
                    Bytes_Debug(bf, s->bytes, s->bytes+s->length-1);
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
    /* Hold the details of a Fmt command in a Struct for
     * later use, starting with a Span argument *p* which is 
     * converted into a void*[].
     */
    FmtLine *ln = (FmtLine *)MemCh_Alloc(m, sizeof(FmtLine));
    ln->type.of = TYPE_FMT_LINE;
    ln->fmt = fmt;
    ln->args = Span_ToArr(m, p);
    return ln;
}

StrVec *Fmt_ToStrVec(MemCh *m, char *fmt, void **args){
    /* Convienience function for placing Fmt contents
     * into a StrVec object by using a temporary Buff
     * object
     */
    Buff *bf = Buff_Make(m, ZERO);
    Fmt(bf, fmt, args); 
    return bf->v;
}

FmtLine *FmtLine_Make(MemCh *m, char *fmt, void **args){
    /* Hold the details of a Fmt command in a Struct for
     * later use. Used extensively by Base.termio.Cli
     * to create interactive console behaviour.
     */
    FmtLine *ln = (FmtLine *)MemCh_Alloc(m, sizeof(FmtLine));
    ln->type.of = TYPE_FMT_LINE;
    ln->fmt = fmt;
    ln->args = args;
    return ln;
}
