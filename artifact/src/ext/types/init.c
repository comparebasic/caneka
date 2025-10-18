#include <external.h>
#include <caneka.h>

status Caneka_Init(MemCh *m){
    status r = READY;
    r |= Caneka_InitBase(m);
    r |= ClassDef_Init(m);
    r |= ExtTypeStrings_Init(m);
    r |= Caneka_LicenceInit(m);
    r |= Parser_Init(m);
    r |= Persist_Init(m);
    r |= Format_Init(m);
    r |= Transp_Init(m);
    r |= Sequence_Init(m);
    r |= Types_ClsInit(m);
    r |= Navigate_ClsInit(m);
    r |= BinSeg_Init(m);
    r |= Task_Init(m);
    r |= Serve_ClsInit(m);
    /*
    r |= Clone_Init(m);
    r |= MemLocal_Init(m);
    r |= Oset_Init(m);
    r |= Enc_Init(m);
    r |= Steps_Init(m);
    r |= Access_Init(m);
    r |= KeyInit(m);
    */
    return r;
}
