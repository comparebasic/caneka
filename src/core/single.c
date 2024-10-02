#include <external.h>
#include <caneka.h>

Single *Single_Ptr(MemCtx *m, void *ptr){
    m = MemCtx_FromHandle((MemHandle *)m);
    Single *sgl = (Single *)MemCtx_Alloc(m, sizeof(Single));
    sgl->type.of = TYPE_WRAPPED_PTR;
    sgl->val.ptr = ptr;
    return sgl;
}
