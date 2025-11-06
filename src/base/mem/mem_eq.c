#include <external.h>
#include <caneka.h>

boolean Iter_Eq(Abstract *a, Abstract *b){
    Error(ErrStream->m, FUNCNAME, FILENAME, LINENUMBER,
        "Iter_Eq Not Implemented", NULL);
    return FALSE;
}

boolean Span_Eq(Abstract *a, Abstract *b){
    Span *pA = (Span *)a;
    Span *pB = (Span *)b;

    if(pA->nvalues != pB->nvalues || pA->max_idx != pB->max_idx){
        return FALSE;
    }

    Iter itA;
    Iter_Init(&itA, pA);
    Iter itB;
    Iter_Init(&itB, pB);
    while(((Iter_Next(&itA)|Iter_Next(&itB)) & END) == 0){
        if(!Equals(itA.value, itB.value)){
            if(a->type.state & DEBUG){
                Abstract *args[] = {
                    (Abstract *)itA.value,
                    (Abstract *)itB.value,
                    NULL
                };
                Debug("^E.Span_Eq false:\n  & vs\n  &^0.\n", args);
            }
            return FALSE;
        }
    }
    if((itB.type.state & END) != (itA.type.state & END)){
        return FALSE;
    }

    return TRUE;
}

status Mem_EqInit(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_SPAN, (void *)Span_Eq);
    r |= Lookup_Add(m, lk, TYPE_ITER, (void *)Iter_Eq);
    return r;
}
