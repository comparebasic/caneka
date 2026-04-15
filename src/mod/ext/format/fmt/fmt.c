#include <external.h>
#include <caneka.h>

Lookup *FormatFmt_Defs = NULL;
status FormatFmt_Init(MemCh *m){
    status r = READY;
    if(FormatFmt_Defs == NULL){
        FormatFmt_Defs = Lookup_Make(m, _TYPE_CORE_END);
        r |= FormatFmt_DefSpan(m, FormatFmt_Defs);
    }
    return r;
}
