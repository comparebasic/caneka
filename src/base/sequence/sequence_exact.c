#include <external.h>
#include <caneka.h>

boolean Span_Exact(Span *a, Span *b){
    if(a->nvalues != b->nvalues){
        return FALSE;
    }
    Iter itA;
    Iter_Init(&itA, a);
    Iter itB;
    Iter_Init(&itB, b);
    while(((Iter_Next(&itA)|Iter_Next(&itB)) & END) == 0){
        if(!Exact(Iter_Get(&itA), Iter_Get(&itB))){
            return FALSE;
        }
    }
    return TRUE;
}
