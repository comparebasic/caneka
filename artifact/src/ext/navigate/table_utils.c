#include <external.h>
#include <caneka.h>

/* untested */
status Table_Merge(Span *tbl, Span *oldTbl){
    Iter it;
    Iter_Init(&it, oldTbl);
    status r = READY;
    while((Iter_Next(&it) & END) == 0){
        Hashed *h = (Hashed *)Iter_Get(&it);
        if(h != NULL){
            h = Hashed_Clone(tbl->m, h);
            if(Table_GetSetHashed(tbl, SPAN_OP_SET, h->key, h->value) != NULL){
                r |= SUCCESS;
            }else{
                r |= ERROR;
                break;
            }
        }
    }
    if(r == READY){
        r |= NOOP;
    }
    return r;
}
