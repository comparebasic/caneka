#include <external.h>
#include "base_module.h"

boolean Ident_Eq(void *_a, void *_b){
    Ident *ident = (Ident *)_a;
    Abstract *b = (Abstract *)_b;

    if(Ident_Check(b) & SUCCESS){
        void *ar[] ={ ident, b, NULL };
        Error(ErrStream->m, FUNCNAME, FILENAME, LINENUMBER,
            "Comparison of Ident and StrVec not yet implemented for @ -> @", ar);
    }else{
        return Equals(ident->name, b);
    }

    return FALSE;
}

boolean Table_Eq(void *a, void *b){
    Table *tblA = (Table *)a;
    Table *tblB = (Table *)b;

    if(tblA->nvalues != tblB->nvalues){
        return FALSE;
    }

    Iter it;
    Iter_Init(&it, tblA);
    void *value = NULL;
    while((Iter_Next(&it) & END) == 0){
        Hashed *h = (Hashed *)it.value;
        if(h != NULL){
            if((value = Table_Get(tblB, h->key)) == NULL){
                if(tblA->type.state & DEBUG){
                    void *args[] = {
                        h->key,
                        NULL
                    };
                    Out("^D.Table_Eq false: missing key & ^d\n", args);
                }
            }
            if(!Equals(h->value, value)){
                if(tblA->type.state & DEBUG){
                    void *args[] = {
                        h->key,
                        h->value,
                        value,
                        tblA,
                        tblB,
                        NULL
                    };
                    Out("^D.Table_Eq false:value mismatch for key $ -> & vs &\n for &\n&^0.\n", args);
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
    r |= Lookup_Add(m, lk, TYPE_IDENT, (void *)Ident_Eq);
    return r;
}
