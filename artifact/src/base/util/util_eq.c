#include <external.h>
#include <caneka.h>

boolean Single_Equals(Single *a, Single *b){
    return a->type.of == b->type.of && a->objType.of == b->objType.of && a->val.value == b->val.value;
}

status Util_EqInit(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_WRAPPED, (void *)Single_Equals);
    return r;
}
