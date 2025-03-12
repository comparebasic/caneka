#include <external.h>
#include <caneka.h>

char *hex_chars = "0123456789abcdef";
static const byte *digits = (byte *)"0123456789";

static inline byte hexCharToOrd(byte b){
    if(b >= '0' && b <= '9'){
        return b - '0';
    }else if(b >= 'a' && b <= 'f'){
        return b - 'a' + 10;
    }else{
        return 0;
    }
}

Str *Str_FromI64(MemCtx *m, i64 i){
    Str *s = Str_Make(m, MAX_BASE10);
    byte *end = s->bytes+(MAX_BASE10-1);
    byte *b = end;

    i32 base = 10;
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

    i64 length = end - b;
    s->alloc = length;
    s->length = length;
    s->bytes = b+1;
    return s;
}

Str *Str_ToHex(MemCtx *m, Str *s){
    if(s->length*2 > STR_MAX){
        Fatal("Requested hex string is too long _i4", (i32)s->length*2);
        return NULL;
    }
    Str *ret = Str_Make(m, s->length*2);
    byte *b = s->bytes;
    byte *end = b+s->length;
    byte *ptr;
    while(b < end){
        byte c = *b;
        byte b2[2];
        b2[0] = hex_chars[((c & 240) >> 4)];
        b2[1] = hex_chars[(c & 15)];
        memcpy(ptr, b2, 2);
        ptr += 2;
        b++;
    }
    return ret;
}
