#include <external.h>
#include <caneka.h>

static boolean _init = FALSE;

status Caneka_Init(MemCh *m){
    if(_init){
        return NOOP;
    }
    _init = TRUE;

    Caneka_InitBase(m);

    ExtTypes_ToSInit(m);
    Seel_Init(m);
    ExtTypeStrings_Init(m);
    Parser_Init(m);
    Persist_Init(m);
    Format_Init(m);
    BinSeg_Init(m);
    Navigate_ToSInit(m, ToStreamLookup);
    Serve_TosInit(m);
    Serve_Init(m);
    Http_TosInit(m);
    Uri_ToSInit(m, ToStreamLookup);
    Node_ClsInit(m);
    IterApi_Init(m);
    Cash_ToSInit(m, ToStreamLookup);

    return m->type.state;
}
