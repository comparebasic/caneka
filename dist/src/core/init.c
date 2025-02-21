#include <external.h>
#include <caneka.h>

status Caneka_Init(MemCtx *m){
    status r = READY;
    r |= archChecks();
    r |= SpanDef_Init();
    r |= DebugStack_Init(m);
    r |= Clone_Init(m);
    r |= Debug_Init(MemCtx_Make());
    r |= Hash_Init(m);
    r |= MemLocal_Init(m);
    r |= Oset_Init(m);
    r |= Enc_Init(m);
    r |= Steps_Init(m);
    r |= KeyInit(m);
    return r;
}

