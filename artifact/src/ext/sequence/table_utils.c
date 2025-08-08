#include <external.h>
#include <caneka.h>

status Table_Underlay(Table *a, Table *b){
    status r = READY;
    Iter it;
    Iter_Init(&it, (Span *)b);
    while((Iter_Next(&it) & END) == 0){
        Hashed *h = Iter_Get(&it);
        if(h != NULL){
            if(Table_GetHashed(a, h->key) == NULL){
                Table_Set(a, h->key, h->value);
                r |= SUCCESS;
            }
        }
    }
    if(r == READY){
        r |= NOOP;
    }
    return r;
}

status Table_Overlay(Table *a, Table *b){
    status r = READY;
    Iter it;
    Iter_Init(&it, (Span *)b);
    while((Iter_Next(&it) & END) == 0){
        Hashed *_h = Iter_Get(&it);
        if(_h != NULL){
            Hashed *h = Table_GetHashed(a, _h->key);
            if(h != NULL){
                h->value = __h->value;
                r |= SUCCESS;
            }else{
                Table_Set(a, _h->key, _h->value);
            }
        }
    }
    if(r == READY){
        r |= NOOP;
    }
    return r;
}
