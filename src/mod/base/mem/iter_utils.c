#include <external.h>
#include "base_module.h"

Iter IT;

void Iter_AddSpan(Iter *it, Span *p){
    Iter it2;
    Iter_Init(&it2, p);
    while((Iter_Next(&it2) & END) == 0){
        Iter_Add(it, it2.value);
    }
}

void Iter_AddSpanRev(Iter *it, Span *p){
    Iter it2;
    Iter_Init(&it2, p);
    while((Iter_Prev(&it2) & END) == 0){
        Iter_Add(it, it2.value);
    }
}

