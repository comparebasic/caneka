#include <external.h>
#include <caneka.h>

status Kve_Capture(word captureKey, int matchIdx, String *s, Abstract *source){
    DebugStack_Push("Kve_Capture", TYPE_CSTR);
    Span *tbl = as(source, TYPE_TABLE);
    if(DEBUG_KVE){
        DPrint((Abstract *)s, DEBUG_KVE, "Kve_Capture(%s): ", Class_ToString(captureKey));
    }

    if(captureKey == KVE_KEY){
       Table_SetKey(tbl, (Abstract *)s); 
    }
    if(captureKey == KVE_KVALUE){
       Table_Set(tbl, (Abstract *)s, (Abstract *)s); 
    }
    if(captureKey == KVE_VALUE){
       Table_SetValue(tbl, (Abstract *)s); 
    }
    if(captureKey == KVE_NUMVALUE){
       Table_SetValue(tbl, (Abstract *)Int_Wrapped(tbl->m, Int_FromString(s))); 
    }
    DebugStack_Pop();
    return NOOP;
}
