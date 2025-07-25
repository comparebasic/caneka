#include <external.h>
#include <caneka.h>

boolean Table_Eq(Abstract *a, Abstract *b){
    Table *tblA = (Table *)a;
    Table *tblB = (Table *)b;

    if(tblA->nvalues != tblB->nvalues){
        return FALSE;
    }

    Iter it;
    Iter_Init(&it, tblA);
    Abstract *value = NULL;
    while((Iter_Next(&it) & END) == 0){
        Hashed *h = (Hashed *)it.value;
        if(h != NULL){
            if((value = Table_Get(tblB, h->key)) == NULL){
                if(a->type.state & DEBUG){
                    Abstract *args[] = {
                        (Abstract *)h->key,
                        NULL
                    };
                    Debug("^D.Table_Eq false: missing key & ^d\n", args);
                }
            }
            if(!Equals(h->value, value)){
                if(a->type.state & DEBUG){
                    Abstract *args[] = {
                        (Abstract *)h->key,
                        (Abstract *)h->value,
                        (Abstract *)value,
                        (Abstract *)tblA,
                        (Abstract *)tblB,
                        NULL
                    };
                    Debug("^D.Table_Eq false:value mismatch for key $ -> & vs &\n for &\n&^0.\n", args);
                }
                return FALSE;
            }
        }
    }

    return TRUE;
}

status Sequence_EqInit(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_TABLE, (void *)Table_Eq);
    return r;
}
