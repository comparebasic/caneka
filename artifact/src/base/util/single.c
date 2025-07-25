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

Single *Ptr_Wrapped(MemCh *m, void *ptr, cls typeOf){
    Single *sgl = (Single *)MemCh_Alloc(m, sizeof(Single));
    boolean isZ = IsZeroed((byte *)sgl, sizeof(Single), FUNCNAME, FILENAME, LINENUMBER);
    sgl->type.of = TYPE_WRAPPED_PTR;
    sgl->objType.of = typeOf;
    sgl->val.ptr = ptr;
    return sgl;
}

Single *MemCount_Wrapped(MemCh *m, util u){
    Single *sgl = (Single *)MemCh_Alloc(m, sizeof(Single));
    sgl->type.of = TYPE_WRAPPED_MEMCOUNT;
    sgl->val.value = u;
    return sgl;
}

Single *Cstr_Wrapped(MemCh *m, char *cstr){
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

Single *I32_Wrapped(MemCh *m, i32 n){
    Single *sgl = (Single *)MemCh_Alloc(m, sizeof(Single));
    sgl->type.of = TYPE_WRAPPED_I32;
    sgl->val.i = n;
    return sgl;
}

Single *I64_Wrapped(MemCh *m, i64 n){
    Single *sgl = (Single *)MemCh_Alloc(m, sizeof(Single));
    sgl->type.of = TYPE_WRAPPED_I64;
    sgl->val.value = (util)n;
    return sgl;
}

Single *I16_Wrapped(MemCh *m, word w){
    Single *sgl = (Single *)MemCh_Alloc(m, sizeof(Single));
    sgl->type.of = TYPE_WRAPPED_I16;
    sgl->val.w = w;
    return sgl;
}

Single *I8_Wrapped(MemCh *m, i8 b){
    Single *sgl = (Single *)MemCh_Alloc(m, sizeof(Single));
    sgl->type.of = TYPE_WRAPPED_I8;
    sgl->val.b = (byte)b;
    return sgl;
}

Single *B_Wrapped(MemCh *m, byte b, cls typeOf, word state){
    Single *sgl = (Single *)MemCh_Alloc(m, sizeof(Single));
    sgl->type.of = TYPE_WRAPPED_BYTE;
    sgl->objType.of = typeOf;
    sgl->objType.state = state;
    sgl->val.b = (byte)b;
    return sgl;
}

Range *Range_Wrapped(MemCh *m, word r){
    Range *w = (Range *)MemCh_Alloc(m, sizeof(Range));
    w->type.of = TYPE_RANGE;
    w->type.range = r;
    return w;
}
