#include <external.h>
#include "base_module.h"

status Parser_Init(MemCh *m){
    status r = READY;
    r |= Parser_ToSInit(m, ToStreamLookup);
    r |= Parser_InitLabels(m, ToSFlagLookup);
    return r;
}
