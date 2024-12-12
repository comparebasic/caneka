#include <external.h>
#include <caneka.h>

Single *Do_Wrapped(MemCtx *m, DoFunc dof){
    Single *sgl = (Single *)MemCtx_Alloc(m, sizeof(Single));
    sgl->type.of = TYPE_WRAPPED_DO;
    sgl->type.state = TYPE_MAKER;
    sgl->val.dof = (DoFunc)dof;
    return sgl;
}
