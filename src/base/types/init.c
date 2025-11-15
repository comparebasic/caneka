#include <external.h>
#include <caneka.h>

static status archChecks(){
    size_t sec = sizeof(i64);
    if(sec != 8){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "Unkown architecture with less than 8 bytes for a util", NULL);
    }
    return ERROR; 
}

status Caneka_InitBase(MemCh *m){
    status r = READY;
    r |= archChecks();
    MemCh *md = MemCh_Make();
    r |= Stream_Init(m, 1, 2);
    r |= Error_Init(m);
    r |= Ifc_Init(md);
    r |= Hash_Init(m);
    r |= Maps_Init(md);
    r |= Debug_Init(md);
    r |= StreamTo_Init(md);
    r |= AnsiStr_Init(md);
    r |= DebugStack_Init(m);
    r |= Clone_Init(m);
    r |= Equals_Init(m);
    r |= Path_Init(md);
    r |= IoUtils_Init(m);
    r |= Exact_Init(m);
    r |= Stash_Init(m);
    return r;
}
