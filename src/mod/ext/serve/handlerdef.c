#include <external.h>
#include <caneka.h>

void HandlerDef_SetExt(MemCh *m, Req *req, Span *path){
    Single *sg = Inst_GetByPath(req->def->extensions, path);
    if(sg != NULL){
        Iter_Set(&req->chain, sg);
        req->type.state |= MORE;
    }
}

HandlerDef *HandlerDef_Make(MemCh *m){
    HandlerDef *def = MemCh_AllocOf(m, sizeof(HandlerDef), TYPE_HANDLER_DEF);
    def->type.of = TYPE_HANDLER_DEF;
    return def;
}
