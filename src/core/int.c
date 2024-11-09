#include <external.h>
#include <caneka.h>

int Int_FromString(String *s){
    int n = atoi((char *)s->bytes);
    return n;
}

Single *Int_Wrapped(MemCtx *m, int n){
    m = MemCtx_FromHandle((MemHandle *)m);
    Single *sgl = (Single *)MemCtx_Alloc(m, sizeof(Single));
    sgl->type.of = TYPE_WRAPPED_UTIL;
    sgl->val.value = (util)n;
    return sgl;
}

Single *I64_Wrapped(MemCtx *m, i64 n){
    m = MemCtx_FromHandle((MemHandle *)m);
    Single *sgl = (Single *)MemCtx_Alloc(m, sizeof(Single));
    sgl->type.of = TYPE_WRAPPED_I64;
    sgl->val.value = (util)n;
    return sgl;
}
