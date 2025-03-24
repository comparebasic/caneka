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

Single *Bool_Wrapped(MemCtx *m, int n){
    Single *sgl = (Single *)MemCtx_Alloc(m, sizeof(Single));
    sgl->type.of = TYPE_WRAPPED_I64;
    sgl->val.value = (util)n;
    return sgl;
}

Single *Do_Wrapped(MemCtx *m, DoFunc dof){
    Single *sgl = (Single *)MemCtx_Alloc(m, sizeof(Single));
    sgl->type.of = TYPE_WRAPPED_DO;
    sgl->type.state = TYPE_MAKER;
    sgl->val.dof = (DoFunc)dof;
    return sgl;
}

Single *Maker_Wrapped(MemCtx *m, Maker mk){
    Single *sgl = (Single *)MemCtx_Alloc(m, sizeof(Single));
    sgl->type.of = TYPE_WRAPPED_FUNC;
    sgl->type.state = TYPE_MAKER;
    sgl->val.ptr = (void *)mk;
    return sgl;
}

Single *Int_Wrapped(MemCtx *m, int n){
    Single *sgl = (Single *)MemCtx_Alloc(m, sizeof(Single));
    sgl->type.of = TYPE_WRAPPED_UTIL;
    sgl->val.value = (util)n;
    return sgl;
}

Single *I64_Wrapped(MemCtx *m, i64 n){
    Single *sgl = (Single *)MemCtx_Alloc(m, sizeof(Single));
    sgl->type.of = TYPE_WRAPPED_I64;
    sgl->val.value = (util)n;
    return sgl;
}

Range *Range_Wrapped(MemCtx *m, word r){
    Range *w = (Range *)MemCtx_Alloc(m, sizeof(Range));
    w->type.of = TYPE_RANGE;
    w->type.range = r;
    return w;
}
