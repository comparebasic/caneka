#include <external.h>
#include <caneka.h>

status Persist_Init(MemCh *m){
    status r = READY;
    r |= Persist_ToSInit(m, ToStreamLookup);
    return r;
}
