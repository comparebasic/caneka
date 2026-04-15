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
    Templ_Init(m);
    DocFunc_Init(m);
    DocComment_Init(m);
    DocComp_Init(m);
    Http_ClsInit(m);
    HttpCtx_Init(m);
    Route_Init(m);
    Www_Init(m);
    WwwPage_Init(m);
    WwwNav_Init(m);
    Gen_Init(m);
    return r;
}
