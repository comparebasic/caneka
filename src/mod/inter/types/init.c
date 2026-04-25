#include <external.h>
#include <caneka.h>
static boolean _init = FALSE;

status Inter_Init(MemCh *m){
    status r = READY;
    if(_init){
        r |= NOOP;
        return r;
    }
    _init = TRUE;

    InterTypeStrings_Init(m);
    return r;
}
