#include <external.h>
#include <caneka.h>

static status archChecks(){
    size_t sec = sizeof(i64);
    if(sec != 8){
        Fatal("Unkown architecture with less than 8 bits for a util\n", TYPE_STRING);
    }
    return ERROR; 
}

status Caneka_Init(MemCtx *m){
    status r = READY;
    r |= archChecks();
    r |= DebugStack_Init(m);
    r |= Clone_Init(m);
    r |= Debug_Init(MemCtx_Make());
    r |= Hash_Init(m);
    r |= MemLocal_Init(m);
    r |= Oset_Init(m);
    r |= Enc_Init(m);
    r |= Steps_Init(m);
    r |= Access_Init(m);
    r |= KeyInit(m);
    return r;
}
