#include <external.h>
#include <caneka.h>

status Parser_Init(MemCh *m){
    status r = READY;
    r |= Parser_ToSInit(m, ToStreamLookup);
    r |= Parser_InitLabels(m, ToSFlagLookup);
    r |= FormatFmt_Init(m);
    r |= Navigate_ToSInit(m, ToStreamLookup);
    r |= Navigate_InitLabels(m, ToSFlagLookup);
    return r;
}
