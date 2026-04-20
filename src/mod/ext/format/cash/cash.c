#include <external.h>
#include <caneka.h>

Span *Cash_Prepare(MemCh *m, Cursor *curs){
    Roebling *rbl = CashParser_Make(m, curs, ZERO);
    Roebling_Run(rbl);
    return ((Iter *)rbl->dest)->p;
}

void Cash_Out(Span *cash, Buff *bf, void *data){
    MemCh *m = cash->m;
    Iter it;
    Iter_Init(&it, cash);
    while((Iter_Next(&it) & END) == 0){
        Abstract *a = Iter_Get(&it);
        if(a->type.of == TYPE_FETCHER){
            a = Fetch(m, (Fetcher *)a, data, NULL);
        }
        ToS(bf, a, a->type.of, ZERO);
    }
}
