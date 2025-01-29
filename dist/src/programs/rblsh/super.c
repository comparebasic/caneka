#include <external.h>
#include <caneka.h>
#include <rblsh.h>

RblShSuper_Make(MemCtx *m, Req *req){
    RblShSuper *super = MemCtx_Alloc(m, sizeof(RblShSuper));
    super->type.of = TYPE_RBLSH_SUPER;
    super->req = req;
    return super;
}
