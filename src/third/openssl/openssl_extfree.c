#include <external.h>
#include <caneka.h>

static status Free_OpenSsl(MemCh *m, void *_a){
    Single *ptr = _a; 
    OPENSSL_free(ptr->val.ptr); 
    return SUCCESS;
}

status OpenSsl_ExtFreeInit(MemCh *m){
   status r = READY;
   ExtFree_Init(m); 
   r |= Lookup_Add(m, ExtFreeLookup, TYPE_ECKEY, (void *)Free_OpenSsl);
   r |= Lookup_Add(m, ExtFreeLookup, TYPE_ECKEY_PUB, (void *)Free_OpenSsl);
   return r;
}
