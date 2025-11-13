#include <external.h>
#include <caneka.h>

Lookup *SealLookup = NULL;
Lookup *SealNameLookup = NULL;

i32 Seal_GetIdx(Table *seal, void *key){
    Hashed *h = Table_GetHashed(seal, key);
    if(h != NULL){
        return h->orderIdx;
    }
    return -1;
}

status Seal_Seal(MemCh *m, Table *seal, Str *name, cls typeOf){
    seal->type.state |= TABLE_SEALED;
    Lookup_Add(m, SealLookup, typeOf, seal);
    Lookup_Add(m, SealNameLookup, typeOf, name);
    return seal->type.state;
}

status Seal_Init(MemCh *m){
    status r = READY;
    if(SealLookup == NULL){
        SealLookup = Lookup_Make(m, TYPE_INSTANCE);
        SealNameLookup = Lookup_Make(m, TYPE_INSTANCE);
        r |= SUCCESS;
    }
    return r;
}
