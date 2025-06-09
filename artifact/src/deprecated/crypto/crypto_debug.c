#include <external.h>
#include <caneka.h>

static void EncPair_Print(Stream *sm, Abstract *a, cls type, boolean extended){
   return; 
}

status CryptoDebug_Init(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_ENC_PAIR, (void *)EncPair_Print);
    return r;
}
