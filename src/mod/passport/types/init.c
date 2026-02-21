#include <external.h>
#include <caneka.h>

status Inter_Init(MemCh *m){
    status r = READY;
    r |= Login_Init(m);
    return r;
}
