#include <external.h>
#include <caneka.h>

status Cdoc_Capture(word captureKey, int matchIdx, String *s, Abstract *source){
    if(DEBUG_CDOC){
        DPrint((Abstract *)s, DEBUG_CDOC, "\x1b[%dmCdoc_Capture(%s): ", CdocRange_ToChars(captureKey));
    }
    
    return NOOP;
}
