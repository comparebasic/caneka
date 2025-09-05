#include <external.h>
#include <caneka.h>

TemplJump *TemplJump_Make(MemCh *m, i32 idx, Fetcher *fch){
    TemplJump *jump = (TemplJump *)MemCh_Alloc(m, sizeof(TemplJump));
    jump->type.of = TYPE_TEMPL_JUMP;
    jump->idx = idx;
    jump->destIdx = jump->skipIdx = jump->tempIdx = -1;
    jump->fch = fch;
    return jump;
}
