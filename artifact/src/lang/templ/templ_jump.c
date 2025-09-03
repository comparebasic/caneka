#include <external.h>
#include <caneka.h>

TemplJump *TemplJump_Make(MemCh *m, i32 idx, Fetcher *fch){
    TemplJump *jump = (TemplJump *)MemCh_Alloc(m, sizeof(TemplJump));
    jump->level;
    jump->type.of = TYPE_TEMPL_JUMP;
    jump->idx = idx;
    jump->destIdx = jump->nestIdx = jump->endIdx = -1;
    jump->fch = fch;
    return jump;
}
