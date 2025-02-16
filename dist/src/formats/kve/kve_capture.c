#include <external.h>
#include <caneka.h>

status Kve_Capture(word captureKey, int matchIdx, String *s, Abstract *source){
    if(DEBUG_KVE){
        DPrint((Abstract *)s, DEBUG_KVE, "Kve_Capture(%s): ", Class_ToString(captureKey));
    }
    return NOOP;
}
