#include <external.h>
#include <caneka.h>

Single *Bool_Wrapped(MemCtx *m, int n){
    Single *sgl = (Single *)MemCtx_Alloc(m, sizeof(Single));
    sgl->type.of = TYPE_WRAPPED_I64;
    sgl->val.value = (util)n;
    return sgl;
}
