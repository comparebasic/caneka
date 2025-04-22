#include <external.h>
#include <caneka.h>

char *hex_chars = "0123456789abcdef";
static byte zeroDigit = (byte)'0';

static inline byte hexCharToOrd(byte b){
    if(b >= '0' && b <= '9'){
        return b - '0';
    }else if(b >= 'a' && b <= 'f'){
        return b - 'a' + 10;
    }else{
        return 0;
    }
}

Str *Str_FromI64(MemCh *m, i64 i){
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

    i64 base = 10;
    i64 val;
    boolean negative = i < 0;
    if(negative){
        i = labs(i);
    }
    *b = '0';
    while(i > 0){
        val = i % base;
        *(b--) = zeroDigit+val;
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

Str *Str_ToHex(MemCh *m, Str *s){
    i32 _len = s->length*2;
    if(_len > STR_MAX){
        void *args[] = { &_len, NULL};
        Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "Requested hex string is too long _i4", args);
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

Str *Str_MemCount(MemCh *m, i64 mem) {
    Str *s = Str_Make(m, STR_DEFAULT);
    Str_AddMemCount(s, mem);
    return s;
}

i64 Str_Trunc(Str *s, i64 amount){
    if(s->type.state & STRING_CONST){
        void *args[] = {s ,NULL};
        Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "Str_Trunc cannot modify a const char * '_t'", args);
    }
    if(amount == 0){
        return 0;
    }else if(amount < 0){
        amount = labs(amount);
        if(s->length < amount){
            return 0;
        }
        byte *ptr = s->bytes+(s->length-amount);
        memset(ptr, 0, amount);
        s->length -= amount;
    }else if(amount > 0){
        if(amount > s->length){
            return 0;
        }
        i64 removed = s->length - amount;
        s->length = amount;
        memset(s->bytes+s->length, 0, removed);
    }
    return amount;
}

Str *Str_Prefixed(MemCh *m, Str *s, Str *prefix){
    if(s->length + prefix->length < s->alloc){
        memmove(s->bytes+prefix->length, s->bytes, s->length);
        memcpy(s->bytes, prefix->bytes, prefix->length);
        return s;
    }else{
        Str *s2 = Str_Make(m, s->length+prefix->length+1);
        memcpy(s2->bytes, prefix->bytes, prefix->length);
        memcpy(s2->bytes+prefix->length, s->bytes, s->length);
        return s2;
    }
}

i64 Str_AddMemCount(Str *s, i64 mem) {
    if(mem == 0){
       return Str_AddCstr(s, "0b"); 
    }
    char *abbrev = "bkmg";
    i64 total = 0;
    i64 div = 1024;
    boolean exactly = FALSE;
    for(int i = 0; i < 4; i++){
       i64 value = mem % div;
       i64 uval = value;
       if(div > 1024){
          uval = value / 1024;
       }
       if(uval > 0){
           if(s->length > 0){
               total += Str_AddCstr(s, " ");
           }
           total += Str_AddI64(s, uval); 
           total += Str_Add(s, (byte *)abbrev+i, 1); 
       }
       div *= 1024;
       mem -= value;
    }

    return total;
}
