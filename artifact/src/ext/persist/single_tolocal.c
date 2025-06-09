#include <external.h>
#include <caneka.h>

status WrappedPtr_ToLocal(MemCh *m, Abstract *a){
    Single *sg = as(a, TYPE_WRAPPED_PTR);
    return MemLocal_SetLocal(m, (Abstract **)&sg->val.ptr);
}

status WrappedPtr_FromLocal(MemCh *m, Abstract *a){
    Single *sg = as(a, TYPE_WRAPPED_PTR);
    return MemLocal_UnSetLocal(m, (Abstract **)&sg->val.ptr);
}

