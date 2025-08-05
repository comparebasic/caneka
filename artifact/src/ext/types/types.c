#include <external.h>
#include <caneka.h>

status ExtTypes_Init(MemCh *m){
    status r = READY;
    r |= ExtTypes_ToSInit(m, ToStreamLookup);
    r |= Types_ExactInit(m);
    return r;
}
