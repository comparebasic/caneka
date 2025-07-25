#include <external.h>
#include <caneka.h>

status Cdoc_Capture(word captureKey, int matchIdx, String *s, Abstract *source){
    if(DEBUG_CDOC){
        DPrint((Abstract *)s, DEBUG_CDOC, "Cdoc_Capture(%s): ", CdocRange_ToChars(captureKey));
    }
    
    return NOOP;
}
