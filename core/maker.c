#include <external.h>
#include <caneka.h>

Single *Maker_Wrapped(MemHandle *mh, Maker mk){
    MemCtx *m = MemCtx_FromHandle(mh);
    Single *sgl = (Single *)MemCtx_Alloc(m, sizeof(Single));
    sgl->type.of = TYPE_WRAPPED_FUNC;
    sgl->type.state = TYPE_MAKER;
    sgl->val.ptr = (void *)mk;
    return sgl;
}
