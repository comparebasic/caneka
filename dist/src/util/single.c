#include <external.h>
#include <caneka.h>
Single *Single_Clone(MemCtx *m, Abstract *og){
    Single *sg = MemCtx_Realloc(m, sizeof(Single), og, sizeof(Single));
    if(sg->type.of == TYPE_WRAPPED_PTR &&
            (sg->val.ptr != NULL && ((sg->val.ptr = Clone(m, sg->val.ptr)) == NULL))){
        return NULL;
    }    
    return sg;
}

Single *Single_Ptr(MemCtx *m, void *ptr){
    Single *sgl = (Single *)MemCtx_Alloc(m, sizeof(Single));
    sgl->type.of = TYPE_WRAPPED_PTR;
    sgl->val.ptr = ptr;
    return sgl;
}
