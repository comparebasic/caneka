#include <external.h>
#include <caneka.h>

static boolean Fetcher_Exact(Fetcher *a, Fetcher *b){
    return FALSE;
}

status Types_ExactInit(MemCh *m){
    status r = READY;
    r |= Lookup_Add(m, ExactLookup, TYPE_FETCHER, (void *)Fetcher_Exact); 
    return r;
}
