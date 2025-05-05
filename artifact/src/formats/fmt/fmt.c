#include <external.h>
#include <caneka.h>

static Lookup *FormatFmt_Defs = NULL;
status FormatFmt_Init(MemCtx *m){
    status r = READY;
    if(FormatFmt_Defs == NULL){
        FormatFmt_Defs = Lookup_Make(m, _TYPE_CORE_END, NULL);
        r |= FormatFmt_DefSpan(m, FormatFmt_Defs);
    }
    return r;
}
