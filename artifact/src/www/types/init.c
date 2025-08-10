#include <external.h>
#include <caneka.h>
#include <www.h>

status Www_Init(MemCh *m){
    status r = READY;
    r |= WwwStrings_Init(m);
    r |= Nav_ClsInit(m);
    return r;
}
