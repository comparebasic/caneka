#include <external.h>
#include <caneka.h>

static boolean _init = FALSE;

status Caneka_Init(MemCh *m){
    status r = READY;
    if(_init){
        r |= NOOP;
        return r;
    }
    _init = TRUE;

    r |= Caneka_InitBase(m);

    r |= ExtTypes_ToSInit(m);
    r |= Seel_Init(m);
    r |= ExtTypeStrings_Init(m);
    r |= Caneka_LicenceInit(m);
    r |= Parser_Init(m);
    r |= Persist_Init(m);
    r |= Format_Init(m);
    r |= Transp_Init(m);
    r |= BinSeg_Init(m);
    r |= Navigate_ClsInit(m);
    r |= Task_Init(m);
    r |= Serve_TosInit(m);
    r |= NodeObj_ClsInit(m);
    r |= DocFunc_Init(m);
    r |= DocComment_Init(m);
    r |= DocComponent_Init(m);

    return r;
}
