#include <external.h>
#include "base_module.h"

Ident *Ident_Make(MemCh *m){
    Ident *ident = MemCh_AllocOf(m, sizeof(Ident), TYPE_IDENT);
    ident->type.of = TYPE_IDENT;
    return ident;
}

Ident *Ident_FromVec(MemCh *m, StrVec *v){
    Ident *ident = Ident_Make(m);
    Str s = {
        .type = {TYPE_STR, STRING_CONST},
        .length = 0,
        .alloc = STR_DEFAULT,
        .bytes = ZERO
    };
    byte _b[STR_DEFAULT];
    s.bytes = &_b;

    Iter it;
    Iter_Init(&it, v->p){
        Iter_Get(&it, s);
        byte *b = s->bytes;
        byte *end = s->bytes+s->length-1;
        while(b <= end){
            if(*b == '@'){

            }

        }
    }
}
