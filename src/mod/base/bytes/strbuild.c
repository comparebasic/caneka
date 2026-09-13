#include <caneka.h>
#include <base_module.h>

StrBuild *StrBuild_From(MemCh *m, byte *b, i16 length){
    StrBuild *sb = (StrBuild *)MemCh_Alloc(m, sizeof(StrBuild));
    sb->type.of = TYPE_STRBUILD;
    sb->p = Span_Make(m);

    status r = READY;

    Iter it;
    Iter_Init(&it, p);

    byte *token = NULL;
    word tlength = 0;

    byte *end = b+length-1;
    while(b <= end){
        byte c = *b;
        if(token != NULL){
            if(c == '}'){
                Str *s = Str_Ref(m, token, tlength, tlength, ZERO);
                Hashed *h = Hashed_Make(m, s);
                s->value = Func_Wrapped(m, NULL, ZERO);
                Iter_Add(&it, h);
                token = NULL;
                tlength = 0;
            }else{
                tlength++;
            }
            goto next;
        }

        if(c == '^'){
            if(b == end){
                Error(m, FUNCNAME, FILENAME, LINENUMBER,
                    "Open color found at end of string", NULL);
                return NULL;
            }
            Str *s = Str_Make(m, ANSI_ESCAPE_MAX);
            Ansi_Consume(m, &b, end - b, s->bytes, s->alloc);
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
        }else if((r & PROCESSING) && c == '{'){
            if(b == end){
                Error(m, FUNCNAME, FILENAME, LINENUMBER,
                    "Open token found at end of string", NULL);
                return NULL;
            }
            Str *s = Str_Ref(m, token, tlength, tlength, ZERO);
            Iter_Add(&it, s);
            token = NULL;
            tlength = 0;

            token = b+1;
            goto next;
        }

        r &= ~PROCESSING;
        goto next;
seg:
        Str *s = Str_Ref(m, token, tlength, tlength, ZERO);
        Iter_Add(&it, s);
        token = NULL;
        tlength = 0;
next:
        b++;
    }

    return sb;
}

Str *StrBuild_ToStr(MemCh *m, StrBuild *sb){
    return NULL;
}

StrVec *StrBuild_ToVec(MemCh *m, StrBuild *sb){
    return NULL;
}

