#include <external.h>
#include <caneka.h>

TemplJump *TemplJump_Make(MemCh *m, cls typeOf, i32 idx, i32 destIdx){
    TemplJump *jump = (TemplJump *)MemCh_Alloc(m, sizeof(TemplJump));
    jump->type.of = TYPE_TEMPL_JUMP;
    jump->jumpType.of = typeOf;
    jump->idx = idx;
    jump->destIdx = destIdx;
    return jump;
}
