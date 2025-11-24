#include <external.h>
#include <caneka.h>

status Inter_Init(MemCh *m){
    status r = READY;
    r |= InterTypeStrings_Init(m);
    r |= Templ_ClsInit(m);
    r |= Http_ClsInit(m);
    r |= HttpCtx_Init(m);
    r |= Route_ClsInit(m);
    r |= Login_Init(m);
    return r;
}
