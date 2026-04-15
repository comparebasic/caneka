#include <external.h>
#include <caneka.h>

StrVec *WwwUtils_UrlSeg(MemCh *m, StrVec *v){
    Iter it;
    Iter_Init(&it, v->p);
    byte _pairs[] = {
        0, 31,
        ' ','*',
        ',', ',',
        '.', '/',
        ':', '<',
        '>', '?',
        '[', '^',
        '`', '`',
        '{', 127,
    };
    Str pairs = {.type = {TYPE_STR, STRING_CONST},
        .length = sizeof(_pairs), 
        .alloc = sizeof(_pairs),
        .bytes = (byte *)&_pairs,
    };
    while((Iter_Next(&it) & END) == 0){
        Str *s = Iter_Get(&it);
        Str_ToLowerFiltered(m, s, &pairs, '-');  
    }
    return v;
}
