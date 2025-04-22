#include <external.h>
#include <caneka.h>

static status archChecks(){
    size_t sec = sizeof(i64);
    if(sec != 8){
        Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "Unkown architecture with less than 8 bytes for a util", NULL);
    }
    return ERROR; 
}

status Caneka_InitBase(MemCh *m){
    status r = READY;
    r |= archChecks();
    MemCh *md = MemCh_Make();
    r |= Core_Init(md);
    r |= Debug_Init(md);
    r |= StrUtils_Init(md);
    r |= DebugStack_Init(m);
    r |= Hash_Init(m);
    return r;
}
