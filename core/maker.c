#include <external.h>
#include <caneka.h>

Single *Maker_Wrapped(MemCtx *m, Maker mk){
    m = MemCtx_FromHandle((MemHandle *)m);
    Single *sgl = (Single *)MemCtx_Alloc(m, sizeof(Single));
    sgl->type.of = TYPE_WRAPPED_FUNC;
    sgl->type.state = TYPE_MAKER;
    sgl->val.ptr = (void *)mk;
    return sgl;
}
