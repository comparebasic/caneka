#include <external.h>
#include <caneka.h>

static status archChecks(){
    size_t sec = sizeof(i64);
    if(sec != 8){
        Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "Unkown architecture with less than 8 bytes for a util");
    }
    return ERROR; 
}

status Caneka_Init(MemCh *m){
    status r = READY;
    r |= archChecks();
    r |= Debug_Init(MemCh_Make());
    r |= DebugStack_Init(m);
    r |= Hash_Init(m);
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
