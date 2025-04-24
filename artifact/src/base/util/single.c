#include <external.h>
#include <caneka.h>

Single *Single_Clone(MemCh *m, Abstract *og){
    Single *sg = MemCh_Realloc(m, sizeof(Single), og, sizeof(Single));
    if(sg->type.of == TYPE_WRAPPED_PTR &&
            (sg->val.ptr != NULL && ((sg->val.ptr = Clone(m, sg->val.ptr)) == NULL))){
        return NULL;
    }    
    return sg;
}

Single *Single_Ptr(MemCh *m, void *ptr){
    Single *sgl = (Single *)MemCh_Alloc(m, sizeof(Single));
    sgl->type.of = TYPE_WRAPPED_PTR;
    sgl->val.ptr = ptr;
    return sgl;
}

Single *Single_Cstr(MemCh *m, char *cstr){
    Single *sgl = (Single *)MemCh_Alloc(m, sizeof(Single));
    sgl->type.of = TYPE_WRAPPED_CSTR;
    sgl->val.ptr = (void *)cstr;
    return sgl;
}

Single *Bool_Wrapped(MemCh *m, int n){
    Single *sgl = (Single *)MemCh_Alloc(m, sizeof(Single));
    sgl->type.of = TYPE_WRAPPED_I64;
    sgl->val.value = (util)n;
    return sgl;
}

Single *Do_Wrapped(MemCh *m, DoFunc dof){
    Single *sgl = (Single *)MemCh_Alloc(m, sizeof(Single));
    sgl->type.of = TYPE_WRAPPED_DO;
    sgl->type.state = TYPE_MAKER;
    sgl->val.dof = (DoFunc)dof;
    return sgl;
}

Single *Maker_Wrapped(MemCh *m, Maker mk){
    Single *sgl = (Single *)MemCh_Alloc(m, sizeof(Single));
    sgl->type.of = TYPE_WRAPPED_FUNC;
    sgl->type.state = TYPE_MAKER;
    sgl->val.ptr = (void *)mk;
    return sgl;
}

Single *Int_Wrapped(MemCh *m, int n){
    Single *sgl = (Single *)MemCh_Alloc(m, sizeof(Single));
    sgl->type.of = TYPE_WRAPPED_UTIL;
    sgl->val.value = (util)n;
    return sgl;
}

Single *I64_Wrapped(MemCh *m, i64 n){
    Single *sgl = (Single *)MemCh_Alloc(m, sizeof(Single));
    sgl->type.of = TYPE_WRAPPED_I64;
    sgl->val.value = (util)n;
    return sgl;
}

Range *Range_Wrapped(MemCh *m, word r){
    Range *w = (Range *)MemCh_Alloc(m, sizeof(Range));
    w->type.of = TYPE_RANGE;
    w->type.range = r;
    return w;
}
