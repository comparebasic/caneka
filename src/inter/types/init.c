#include <external.h>
#include <caneka.h>

status Inter_Init(MemCh *m){
    status r = READY;
    InterTypeStrings_Init(m);
    Templ_Init(m);
    DocFunc_Init(m);
    DocComment_Init(m);
    DocComp_Init(m);
    Http_ClsInit(m);
    HttpCtx_Init(m);
    Route_Init(m);
    WwwPage_Init(m);
    WwwNav_Init(m);
    return r;
}
