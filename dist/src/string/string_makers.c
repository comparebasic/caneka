#include <external.h>
#include <caneka.h>

char *hex_chars = "0123456789abcdef";

static const byte *digits = (byte *)"0123456789";

static byte hexCharToOrd(byte b){
    if(b >= '0' && b <= '9'){
        return b - '0';
    }else if(b >= 'a' && b <= 'f'){
        return b - 'a' + 10;
    }else{
        return 0;
    }
}

static int _String_FromI64(MemCtx *m, i64 i, byte buff[]){
    memset(buff, 0, MAX_BASE10+1);


    int base = 10;
    int pows = 0;
    int position = MAX_BASE10-1;
    i64 val;
    byte digit = digits[0];
    boolean found = FALSE;
    boolean negative = i < 0;
    if(negative){
        i = labs(i);
    }
    while(i > 0){
        found = TRUE;
        val = i % base;
        digit = digits[val];
        buff[position] = digit;
        i -= val;
        i /= 10;
        position--;
    }

    if(!found){
        buff[position] = digits[0];
        position--;
    }

    if(negative){
        buff[position] = '-';
        position--;
    }

    return position;
}

String *String_ToEscaped(MemCtx *m, String *s){
    int length = 0;
    byte b;
    for(int i = 0; i < s->length; i++){
        b = s->bytes[i];
        if(b > 31 && b < 127){
            length++;
        }else{
            if(
                b == '\r' ||
                b == '\t' ||
                b == '\n' ||
                b == '\\' ||
                b == KEY_ESCAPE || 
                b < 10
            ){
                length += 2;
            }else if (b < 100){
                length += 3;
            }else{
                length += 4;
            }
        }
    }
    String *s2 = String_Init(m, length);
    int position;
    for(int i = 0; i < s->length; i++){
        b = s->bytes[i];
        if(b > 31 && b < 127){
            String_AddBytes(m, s2, &b, 1);
        }else{
            String_AddBytes(m, s2, bytes("\\"), 1);
            if(b == '\r'){
                String_AddBytes(m, s2, bytes("r"), 1);
            }else if(b == '\n'){
                String_AddBytes(m, s2, bytes("n"), 1);
            }else if(b == '\\'){
                String_AddBytes(m, s2, bytes("\\"), 1);
            }else if(b == '\t'){
                String_AddBytes(m, s2, bytes("t"), 1);
            }else if(b == KEY_ESCAPE){
                String_AddBytes(m, s2, bytes("e"), 1);
            }else{
                String_AddInt(m, s2, b);
            }
        }
    }

    return s2;
}

String *String_FromInt(MemCtx *m, int i){
    byte buff[MAX_BASE10+1];
    int position = _String_FromI64(m, (i64)i, buff);
    return String_Make(m, buff+position+1); 
}

status String_AddI64(MemCtx *m, String *s, i64 n){
    byte buff[MAX_BASE10+1];
    int position = _String_FromI64(m, n, buff);
    return String_AddBytes(m, s, buff+position+1, MAX_BASE10-position-1);
}

String *String_FromI64(MemCtx *m, i64 i){
    byte buff[MAX_BASE10+1];
    int position = _String_FromI64(m, i, buff);
    return String_Make(m, buff+position+1); 
}

status String_AddInt(MemCtx *m, String *s, int i){
    byte buff[MAX_BASE10+1];
    int position = _String_FromI64(m, (i64)i, buff);
    return String_AddBytes(m, s, buff+position+1, MAX_BASE10-position-1);
}

char *String_ToChars(MemCtx *m, String *s){
    if(s == NULL){
        return NULL;
    }
    i64 l = String_Length(s); 
    if(l > MEM_SLAB_SIZE){
        Fatal("Error, unable to allocate a fixed block larger than the memblock size", TYPE_STRING);
        return NULL;
    }

    char *cstr = (char *)MemCtx_Alloc(m, l+1);
    i64 offset = 0;
    while(s != NULL){
        memcpy(cstr+offset, s->bytes, s->length);
        offset += s->length;
        s = String_Next(s);
    }

    cstr[offset] = '\0';
    return cstr;
}

String *String_ToHex(MemCtx *m, String *s){
    String *ret = String_Init(m, s->length*2);
    while(s != NULL){
        byte *b = s->bytes;
        byte *end = b+s->length;
        while(b < end){
            byte c = *b;
            byte b2[3];
            b2[2] = 0;
            b2[0] = hex_chars[((c & 240) >> 4)];
            b2[1] = hex_chars[(c & 15)];
            String_AddBytes(m, ret, b2, 2);
            b++;
        }
        s = String_Next(s);
    };
    return ret;
}

status String_MakeUpper(String *s){
    int delta = 'A' - 'a';
    while(s != NULL){
        byte *b = s->bytes;
        byte *end = b+s->length;
        while(b < end){
            byte c = *b;
            if(c >= 'a' && c <= 'z'){
                *b = c + delta;
            }
            b++;
        }
        s = String_Next(s);
    };
    return SUCCESS;
}


status String_MakeLower(String *s){
    int delta = 'A' - 'a';
    while(s != NULL){
        byte *b = s->bytes;
        byte *end = b+s->length;
        while(b < end){
            byte c = *b;
            if(c >= 'A' && c <= 'Z'){
                *b = c - delta;
            }
            b++;
        }
        s = String_Next(s);
    };
    return SUCCESS;
}

String *String_ToCamel(MemCtx *m, String *s){
    boolean first = TRUE;
    int delta = 'A' - 'a';
    String *ret = String_Init(m, s->length);
    while(s != NULL){
        byte *b = s->bytes;
        byte *end = b+s->length;
        while(b < end){
            byte c = *b;
            if(first == TRUE && c >= 'a' && c <= 'z'){
                *b = c + delta;
                String_AddBytes(m, ret, b, 1);
                first = FALSE;
            }else if((c >= 'a' && c <= 'z') || c >= 'A' && c <= 'Z' || c == '_' || c == '-' || (c >= '0' && c <= '9')){
                String_AddBytes(m, ret, b, 1);
            }else{
                first = TRUE;
            }
            b++;
        }
        s = String_Next(s);
    };

    return ret;
}

String *String_FromHex(MemCtx *m, String *s){
    String *ret = String_Init(m, s->length/2);
    byte b2[2];
    byte b = 0;
    int offset = 0;
    while(s != NULL){
        while(offset < s->length){
            memcpy(b2, s->bytes+offset, 2);
            byte a0 = b2[0];
            byte a1 = b2[1];
            b2[0] = hexCharToOrd(b2[0]);
            b2[1] = hexCharToOrd(b2[1]);
            b = (b2[0] << 4) | b2[1];
            String_AddBytes(m, ret, bytes(&b), 1);
            offset += 2;
        }
        s = String_Next(s);
    };
    ret->type.state |= FLAG_STRING_BINARY;
    return ret;
}

status String_AddBitPrint(MemCtx *m, String *s, byte *bt, size_t length, boolean extended){
    status r = READY;
    for(int i = length-1; i >= 0;i--){
        byte b = bt[i];
        if(extended){
            String_AddInt(m, s, (int)b); 
            String_AddBytes(m, s, bytes("="), 1);
        }
        for(int j = 7; j >= 0;j--){
            r |= SUCCESS;
            byte x = (b & (1 << j)) ? '1' : '0';
            String_AddBytes(m, s, &x, 1); 
        }
        if(extended){
            String_AddBytes(m, s, bytes(" "), 1);
        }
    }
    return r;
}

status String_AddMemCount(MemCtx *m, String *s, i64 mem) {
    if(mem == 0){
       String_AddBytes(m, s, bytes("0b"), 2); 
       return NOOP;
    }
    char *abbrev = "bkmg";
    i64 div = 1024;
    for(int i = 0; i < 4; i++){
       if(i > 0){
           String_AddBytes(m, s, bytes(" "), 1); 
       }
       i64 value = mem % div;
       i64 uval = value;
       if(div > 1024){
          uval = value / 1024;
       }
       if(uval == 0){
            continue;
       }
       String_AddInt(m, s, uval); 
       String_AddBytes(m, s, bytes(abbrev+i), 1); 
       div *= 1024;
       mem -= value;
    }

    return SUCCESS;
}

status String_AddAsciiSrc(MemCtx *m, String *s, byte c) {
    if(c >= 32 && c <= 126){
        char *cstr = "'";
        String_AddBytes(m, s, bytes(cstr), strlen(cstr));
        String_AddBytes(m, s, &c, 1);
        cstr = "'";
        String_AddBytes(m, s, bytes(cstr), strlen(cstr));
    }else{
        String_AddInt(m, s, c);
    }

    return SUCCESS;
}

status String_ToSlab(String *a, void *sl, size_t sz) {
    if(String_Length(a) > sz){
        return ERROR;
    }
    if(a == NULL){
        Fatal("Error string is NULL", TYPE_STRING_CHAIN);
    }
    size_t remaining = sz;

    String *seg = a;
    void *p = sl;

    while(seg != NULL && remaining > 0){
        memcpy(p, seg->bytes, seg->length);
        remaining -= seg->length;
        p += seg->length;
        seg = String_Next(seg);
    }

    if(remaining > 0){
        memset(p, 0, remaining);
    }

    if(seg != NULL){
        return ERROR;
    }

    return SUCCESS;
}

