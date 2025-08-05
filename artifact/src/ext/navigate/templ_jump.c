#include <external.h>
#include <caneka.h>

TemplJump *TemplJump_Make(MemCh *m, i32 idx, i32 destIdx){
    TemplJump *jump = (TemplJump *)MemCh_Alloc(m, sizeof(TemplJump));
    jump->type.of = TYPE_TEMPL_JUMP;
    jump->idx = idx;
    jump->destIdx = destIdx;
    return jump;
}
