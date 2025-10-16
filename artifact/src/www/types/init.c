#include <external.h>
#include <caneka.h>

status Www_Init(MemCh *m){
    status r = READY;
    r |= WwwStrings_Init(m);
    r |= Nav_ClsInit(m);
    r |= Page_ClsInit(m);
    r |= Route_ClsInit(m);
    return r;
}
