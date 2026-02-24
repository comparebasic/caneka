#include <external.h>
#include "base_module.h"

static status archChecks(){
    size_t sec = sizeof(i64);
    if(sec != 8){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "Unkown architecture with less than 8 bytes for a util", NULL);
    }
    return ERROR; 
}

status Caneka_InitBase(MemCh *m){
    status r = READY;
    archChecks();
    MemCh *md = MemCh_Make();
    Core_Init(m);
    Error_Init(m);
    Ifc_Init(md);
    Empty_Init(m);
    Hash_Init(m);
    Maps_Init(md);
    Debug_Init(md);
    StreamTo_Init(md);
    AnsiStr_Init(md);
    DebugStack_Init(m);
    Termio_ToSInit(m);
    Args_Init(m);
    Clone_Init(m);
    Equals_Init(m);
    Path_Init(md);
    IoUtils_Init(m);
    Exact_Init(m);
    Stash_Init(m);
    Core_Direct(m, 1, 2);
    return r;
}
