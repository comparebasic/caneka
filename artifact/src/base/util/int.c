#include <external.h>
#include <caneka.h>

i32 Int_FromStr(Str *s){
    i64 n = I64_FromStr(s);
    if(n <= INT_MAX){
        return (i32)n;
    }
    s->type.state |= ERROR;
    return 0;
}

i64 I64_FromStr(Str *s){
    if(s->length == 0){
        s->type.state |= NOOP;
        return 0;
    }
    byte *b = s->bytes+(s->length-1);
    byte *start = s->bytes;
    i64 pow = 1;
    i64 n = 0;
    while(b >= start){
        byte c = *b;
        if(c >= '0' &&  c <= '9'){
            n += (c-'0') * pow;
        }else if(b == s->bytes && c == '-'){
            n = -n;
        }else{
            s->type.state |= ERROR;
            return 0;
        }

        if(b == start){
            break;
        }else{
            b--;
            pow *= 10;
        }
    }
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
