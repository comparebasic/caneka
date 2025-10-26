#include <external.h>
#include <caneka.h>

Stream *DebugOut = NULL;
MemCh *_debugM = NULL;
Lookup *TypeStringRanges = NULL;

status Debug_Init(MemCh *m){
    if(_debugM == NULL){
        _debugM = m;
    }
    if(DebugOut == NULL){
        DebugOut = Stream_MakeToFd(m, 0, NULL, 0);
    }
    if(TypeStringRanges == NULL){
        TypeStringRanges = Lookup_Make(m, _TYPE_ZERO);
        DebugTypeStrings_Init(m);
    }
    m->level++;
    return NOOP;
}
