#include <external.h>
#include <base_module.h>

Templ *Templ_FromCstr(MemCh *m, char *cstr){
    Str *s = S(m, cstr);
    return Templ_From(m, s->bytes, s->length);
}

Templ *Templ_From(MemCh *m, byte *b, i16 length){
    Templ *templ = (Templ *)MemCh_Alloc(m, sizeof(Templ));
    templ->type.of = TYPE_TEMPL;
    templ->p = Span_Make(m);

    Str *s = NULL;
    status r = READY;

    Iter it;
    Iter_Init(&it, templ->p);

    byte *token = b;
    word tlength = 0;

    i32 idx = 0;

    byte *end = b+length-1;
    while(b <= end){
        byte c = *b;

        if(b == end && (c == '^' || c == '\\' || c == '%' || c == '@' || c == '&')){
            s = Str_Ref(m, b, length, length, ZERO);
            void *ar[] = {s, NULL};
            Error(m, FUNCNAME, FILENAME, LINENUMBER,
                "Open or escape found at end of string", ar);
            return NULL;
        }

        if(r & (PROCESSING|SUCCESS)){
            if(c == '}'){
                Str *s = Str_Ref(m, token, tlength, tlength, ZERO);
                Hashed *h = Hashed_Make(m, s);
                r &= ~(PROCESSING|SUCCESS);
                h->objType.state = r;
                h->value = Func_Wrapped(m, NULL, ZERO);
                h->orderIdx = idx++;
                Iter_Add(&it, h);
                token = b+1;
                tlength = 0;
                b++;
                r = READY;
                continue;
            }else if((r & PROCESSING) && c == '{'){
                s = Str_Ref(m, token, tlength, tlength, ZERO);
                Iter_Add(&it, s);
                token = b+1;
                tlength = 0;
                r |= SUCCESS;
                goto next;
            }else if((r & SUCCESS) == 0){
                Hashed *h = Hashed_Make(m, NULL);
                r &= ~PROCESSING;
                h->objType.state = r;
                h->value = Func_Wrapped(m, NULL, ZERO);
                h->orderIdx = idx++;
                Iter_Add(&it, h);
                r = READY;
            }
        }

        if(c == '\\'){
            if(b == end){
                break;
            }else{
                b++;
            }
        }else if(c == '^'){
            b++;
            s = Str_Make(m, ANSI_ESCAPE_MAX);
            s->type.state |= STRING_BINARY;
            s->length = Ansi_Consume(m, (char **)&b, end - b, s->bytes, s->alloc);
            Iter_Add(&it, s);
            token = b+1;
            goto next;
        }else if(c == '%'){
            r = PROCESSING;
            goto seg;
        }else if(c == '@'){
            r = (PROCESSING|MORE);
            goto seg;
        }else if(c == '&'){
            r = (PROCESSING|DEBUG);
            goto seg;
        }

        tlength++;

        r &= ~PROCESSING;
        goto next;
seg:
        s = Str_Ref(m, token, tlength, tlength, ZERO);
        Iter_Add(&it, s);
        token = b+1;
        tlength = 0;
next:
        b++;
    }

    return templ;
}

Str *Templ_ToStr(MemCh *m, Templ *templ){
    return NULL;
}

StrVec *Templ_ToVec(MemCh *m, Templ *templ){
    return NULL;
}

