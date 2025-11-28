#include <external.h>
#include <caneka.h>
#include "../module.h"

boolean Str_ExactStr(Str *a, Str *b){
    return a->type.state == b->type.state && Str_EqualsStr((Str *)a, (Str *)b);
}

boolean StrVec_ExactStrVec(StrVec *a, StrVec *b){
    if(a->total != b->total || a->p->nvalues != b->p->nvalues){
        return FALSE;
    }
    Iter itA;
    Iter_Init(&itA, a->p);
    Iter itB;
    Iter_Init(&itB, b->p);
    while(((Iter_Next(&itA)|Iter_Next(&itB)) & END) == 0){
        Str *sA = (Str *)Iter_Get(&itA);
        Str *sB = (Str *)Iter_Get(&itB);
        if(!Str_ExactStr(sA, sB)){
            return FALSE;
        }
    }
    return TRUE;
}
