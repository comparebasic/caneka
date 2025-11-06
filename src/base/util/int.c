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
