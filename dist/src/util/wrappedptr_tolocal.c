#include <external.h>
#include <caneka.h>

status WrappedPtr_ToLocal(MemCtx *m, Abstract *a){
    Single *sg = as(a, TYPE_WRAPPED_PTR);
    return MemLocal_SetLocal(m, (Abstract **)&sg->val.ptr);
}

status WrappedPtr_FromLocal(MemCtx *m, Abstract *a){
    Single *sg = as(a, TYPE_WRAPPED_PTR);
    return MemLocal_UnSetLocal(m, (LocalPtr *)&sg->val.ptr);
}
