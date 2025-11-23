#include <external.h>
#include <caneka.h>

Str *Str_FromI64Pad(MemCh *m, i64 n, i32 pad){
    if(pad > MAX_BASE10){
        return NULL;
    }
    Str *s = Str_Make(m, MAX_BASE10);
    byte *b = s->bytes;
    i64 length = Str_I64OnBytes(&b, n);
    if(length < pad){
        word diff = pad-length;
        while(--diff){
            *(--b) = '0';
            s->length++;
            s->alloc++;
            s->bytes = b;
        }
        s->length++;
        s->alloc++;
    }

    return s;
}

Str *Str_FromI64(MemCh *m, i64 i){
    Str *s = Str_Make(m, MAX_BASE10);
    Str_AddI64(s, i);
    return s;
}

i64 Str_I64OnBytes(byte **_b, i64 i){
    byte *end = (*_b)+(MAX_BASE10-1);
    byte *b = end;

    i64 base = 10;
    i64 val;
    boolean negative = i < 0;
    if(negative){
        i = labs(i);
    }
    *b = '0';
    while(i > 0){
        val = i % base;
        *(b--) = '0'+val;
        i -= val;
        i /= base;
    }

    if(negative){
        *(b--) = '-';
    }else if(b == end){
        *(b--) = '0';
    }

    *_b = b+1;
    return end - b;
}

i64 Str_AddIByte(Str *s, byte i){
    byte _b[MAX_BASE10];
    if(s->alloc < s->length+4){
        s->type.state |= ERROR;
        return 0;

    }

    byte *b = _b;
    i64 n = i;
    i64 length = Str_I64OnBytes(&b, n);
    return Str_Add(s, b, length);
}


i64 Str_AddI64(Str *s, i64 i){
    byte _b[MAX_BASE10];
    if(s->alloc < s->length+MAX_BASE10){
        s->type.state |= ERROR;
        return 0;

    }

    byte *b = _b;
    i64 length = Str_I64OnBytes(&b, i);
    return Str_Add(s, b, length);
}



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
