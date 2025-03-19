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
    Str_AddI64(s, i);
    return s;
}

i64 Str_AddI64(Str *s, i64 i){
    byte _b[MAX_BASE10];
    if(s->alloc < s->length+MAX_BASE10){
        s->type.state |= ERROR;
        return 0;

    }
    byte *end = _b+(MAX_BASE10-1);
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
    return Str_Add(s, b+1, length);
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

i64 Str_AddMemCount(Str *s, i64 mem) {
    if(mem == 0){
       return Str_AddCStr(s, "0b"); 
    }
    char *abbrev = "bkmg";
    i64 total = 0;
    i64 div = 1024;
    for(int i = 0; i < 4; i++){
       if(i > 0){
           total += Str_AddCStr(s, " ");
       }
       i64 value = mem % div;
       i64 uval = value;
       if(div > 1024){
          uval = value / 1024;
       }
       if(uval == 0){
            continue;
       }
       total += Str_AddI64(s, uval); 
       total += Str_Add(s, (byte *)abbrev+i, 1); 
       div *= 1024;
       mem -= value;
    }

    return total;
}
