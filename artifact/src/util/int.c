#include <external.h>
#include <caneka.h>

int Int_FromString(String *s){
    int n = atoi((char *)s->bytes);
    return n;
}

i64 I64_FromString(String *s){
    i64 n = strtol((char *)s->bytes, NULL, 10);
    return n;
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
