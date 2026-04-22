#include <external.h>
#include <caneka.h>

Span *Cash_Prepare(MemCh *m, Cursor *curs){
    Roebling *rbl = CashParser_Make(m, curs, ZERO);
    Roebling_Run(rbl);
    Roebling_Finalize(rbl, NULL, 0);
    return ((Iter *)rbl->dest)->p;
}

void Cash_Out(Span *cash, Buff *bf, void *data){
    MemCh *m = cash->m;
    Iter it;
    Iter_Init(&it, cash);
    while((Iter_Next(&it) & END) == 0){
        Abstract *a = Iter_Get(&it);
        if(a->type.of == TYPE_FETCHER){
            Fetcher *fch = (Fetcher *)a;
            Fetch(m, fch, data, NULL);
            a = fch->value;
            Abstract *tg = Iter_GetByIdx(&fch->targets, fch->targets.p->max_idx);
            if(tg->type.of == TYPE_CASH_JUMP){
                Jump *jmp = (Jump *)tg; 
                if(jmp->objType.of == CASH_IF && a == NULL && jmp->idx != -1){
                    Iter_GetByIdx(&it, jmp->idx-1);
                }
                continue;
            }
        }

        if(a != NULL && a->type.of != TYPE_CASH_JUMP){
            ToS(bf, a, a->type.of, ZERO);

        }
    }
}
