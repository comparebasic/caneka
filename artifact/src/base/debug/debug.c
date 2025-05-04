#include <external.h>
#include <caneka.h>

Stream *DebugOut = NULL;
MemCh *_debugM = NULL;
Str **stateLabels = NULL;

status Debug_Init(MemCh *m){
    if(_debugM == NULL){
        _debugM = m;
    }
    if(DebugOut == NULL){
        DebugOut = Stream_MakeToFd(m, 0, NULL, 0);
    }
    if(stateLabels == NULL){
        stateLabels = (Str **)Arr_Make(m, 17);
        stateLabels[0] = Str_CstrRef(m, "ZERO/READY");
        stateLabels[1] = Str_CstrRef(m, "SUCCESS");
        stateLabels[2] = Str_CstrRef(m, "ERROR");
        stateLabels[3] = Str_CstrRef(m, "NOOP");
        stateLabels[4] = Str_CstrRef(m, "DEBUG");
        stateLabels[5] = Str_CstrRef(m, "MORE");
        stateLabels[6] = Str_CstrRef(m, "CONTINUE");
        stateLabels[7] = Str_CstrRef(m, "END");
        stateLabels[8] = Str_CstrRef(m, "PROCESSING");
        stateLabels[9] = Str_CstrRef(m, "CLS_FLAG_ALPHA");
        stateLabels[10] = Str_CstrRef(m, "CLS_FLAG_BRAVO");
        stateLabels[11] = Str_CstrRef(m, "CLS_FLAG_CHARLIE");
        stateLabels[12] = Str_CstrRef(m, "CLS_FLAG_DELTA");
        stateLabels[13] = Str_CstrRef(m, "CLS_FLAG_ECHO");
        stateLabels[14] = Str_CstrRef(m, "CLS_FLAG_FOXTROT");
        stateLabels[15] = Str_CstrRef(m, "CLS_FLAG_GOLF");
        stateLabels[16] = Str_CstrRef(m, "CLS_FLAG_HOTEL");
    }
    m->type.range++;
    return NOOP;
}
