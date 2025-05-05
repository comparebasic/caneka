#include <external.h>
#include <caneka.h>

Stream *DebugOut = NULL;
MemCh *_debugM = NULL;

status Debug_Init(MemCh *m){
    if(_debugM == NULL){
        _debugM = m;
    }
    if(DebugOut == NULL){
        DebugOut = Stream_MakeToFd(m, 0, NULL, 0);
    }
    m->type.range++;
    return NOOP;
}
