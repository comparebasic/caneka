#include <external.h>
#include <caneka.h>

Lookup *SeelLookup = NULL;
Lookup *SeelNameLookup = NULL;

i32 Seel_GetIdx(Table *seel, void *key){
    Hashed *h = Table_GetHashed(seel, key);
    if(h != NULL){
        return h->orderIdx;
    }
    return -1;
}

status Seel_Seel(MemCh *m, Table *seel, Str *name, cls typeOf){
    seel->type.state |= TABLE_SEALED;
    Lookup_Add(m, SeelLookup, typeOf, seel);
    Lookup_Add(m, SeelNameLookup, typeOf, name);
    return seel->type.state;
}

status Seel_Init(MemCh *m){
    status r = READY;
    if(SeelLookup == NULL){
        SeelLookup = Lookup_Make(m, TYPE_INSTANCE);
        SeelNameLookup = Lookup_Make(m, TYPE_INSTANCE);
        r |= SUCCESS;
    }
    return r;
}
