#include <external.h>
#include <caneka.h>

byte *b64_chars = bytes("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/");
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

    return position;
}

String *String_Clone(MemCtx *m, String *s){
    String *s2 = String_Init(m, s->length);
    String_Add(m, s2, s);
    return s2;
}

String *String_Init(MemCtx *m, int expected){
    size_t sz = sizeof(StringMin);
    cls type = TYPE_STRING_FIXED;
    if(expected < 0 || expected > STRING_FULL_SIZE){
        sz = sizeof(String);
        type = TYPE_STRING_CHAIN;
    }else if(expected > STRING_FIXED_SIZE){
        sz = sizeof(StringFull);
        type = TYPE_STRING_FULL;
    }
    String *s = (String *)MemCtx_Alloc(m, sz);
    s->type.of = type;
    s->type.state |= (m->type.state & LOCAL_PTR);

    return s;
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
        byte buff[MAX_BASE10+1];
        if(b > 31 && b < 127){
            String_AddBytes(m, s2, &b, 1);
            continue;
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
    return String_From(m, buff+position+1); 
}

status String_AddI64(MemCtx *m, String *s, i64 n){
    byte buff[MAX_BASE10+1];
    int position = _String_FromI64(m, n, buff);
    return String_AddBytes(m, s, buff+position+1, MAX_BASE10-position-1);
}

String *String_FromI64(MemCtx *m, i64 i){
    byte buff[MAX_BASE10+1];
    int position = _String_FromI64(m, i, buff);
    return String_From(m, buff+position+1); 
}

status String_AddInt(MemCtx *m, String *s, int i){
    byte buff[MAX_BASE10+1];
    int position = _String_FromI64(m, (i64)i, buff);
    return String_AddBytes(m, s, buff+position+1, MAX_BASE10-position-1);
}

char *String_ToChars(MemCtx *m, String *s){
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

String *String_Make(MemCtx *m, byte *bytes){
    int length = strlen((char *)bytes);
    String *s = String_Init(m, length); 
    String_AddBytes(m, s, bytes, length);
    return s;
}

String *String_MakeFixed(MemCtx *m, byte *bytes, int length){
    String *s = String_Init(m, length); 
    String_AddBytes(m, s, bytes, length);

    return s;
}

String *String_From(MemCtx *m, byte *bytes){
    return String_Make(m, bytes);
}

String *String_ToB64(MemCtx *m, String *s){
    B64Ctx bctx;
    memset(&bctx, 0, sizeof(B64Ctx));
    String *bs = String_Init(m, ((s->length/3)*4) + 3);

    int padding = 0;
    while(s != NULL){
        int remaining = s->length;
        int offset = 0;
        while(remaining > 0){
            if(remaining < 3){
                padding = 3 - remaining;
                memset(&bctx.byte3, 0, 3);
                for(int i = 0; i < remaining; i++){
                    bctx.byte3[i] = s->bytes[offset];
                    offset++;
                    i++;
                    remaining--;
                }
                remaining = 0;
            }else{
                memcpy(&bctx.byte3, s->bytes+offset, 3);
                offset += 3;
                remaining -= 3;
            }

            memset(&bctx.out, 0, 4);
            bctx.out[0] = bctx.byte3[0] >> 2; 
            bctx.out[1] = ((bctx.byte3[0] & 3) << 4 ) | ((bctx.byte3[1] & 240) >> 4); 
            bctx.out[2] = ((bctx.byte3[1] & 15) << 2 ) | ((bctx.byte3[2] & 192) >> 6); 
            bctx.out[3] = (bctx.byte3[2] & 63); 

            for(int j = 0; j < 4-padding; j++){
                String_AddBytes(m, bs, &(b64_chars[bctx.out[j]]), 1);
            }
        }

        for(int i = 0; i < padding; i++){
            String_AddBytes(m, bs, bytes("="), 1);
        }


        s = String_Next(s);
    }

    return bs;
}

String *String_ToHex(MemCtx *m, String *s){
    String *ret = String_Init(m, s->length*2);
    while(s != NULL){
        byte *b = s->bytes;
        byte *end = b+s->length;
        while(b < end){
            byte c = *b;
            byte b2[2];
            b2[0] = hex_chars[((c & 240) >> 4)];
            b2[1] = hex_chars[(c & 15)];
            String_AddBytes(m, ret, b2, 2);
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

static byte b64CharValue(byte b){

    if(b >= 'A' && b <= 'Z'){
        return b - 'A';
    }else if(b >= 'a' && b <= 'z'){
        return b - 'a' + 26;
    }else if(b >= '0' && b <= '9'){
        return b - '0' + 52;
    }else if (b == '+'){
        return 62;
    }else if (b == '/'){
        return 63;
    }else if(b == '='){
        return 0;
    }else{
        Fatal("Character out of range for Base64", TYPE_STRING);
        return 0;
    }
}

String *String_FromB64(MemCtx *m, String *bs){
    B64Ctx bctx;
    memset(&bctx, 0, sizeof(B64Ctx));
    String *s = String_Init(m, (bs->length / 4) * 3);

    int padding = 0;
    while(bs != NULL){
        int remaining = bs->length;
        int offset = 0;
        while(remaining > 0){
            if(remaining < 4){
                return NULL;
            }else if(remaining == 4){
                for(int i = 0; i < 4; i++){
                   if(*(bs->bytes+offset+i) ==  '='){
                        padding++;
                   }
                }
                remaining = 0;
            }else{
                memcpy(&bctx.out, bs->bytes+offset, 4);
                offset += 4;
                remaining -= 4;
            }

            memset(&bctx.byte3, 0, 3);
            byte b0 = b64CharValue(bctx.out[0]);
            byte b1 = b64CharValue(bctx.out[1]);
            byte b2 = b64CharValue(bctx.out[2]);
            byte b3 = b64CharValue(bctx.out[3]);

            bctx.byte3[0] = (b0 << 2) | (b1 >> 4);
            bctx.byte3[1] = '0';
            bctx.byte3[2] = '0';

            String_AddBytes(m, s, bctx.byte3, 3-padding);
        }

        bs = String_Next(bs);
    }

    return s;
}

status String_Add(MemCtx *m, String *a, String *b) {
    status r = READY;
    while(b != NULL && r != ERROR){
        a->type.state |= (b->type.state & FLAG_STRING_BINARY);
        r = String_AddBytes(m, a, b->bytes, b->length);
        b = String_Next(b);
    }
    return r;
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
status String_AddBytes(MemCtx *m, String *a, byte *chars, int length) {
    if(a == NULL){
        Fatal("Error string is NULL", TYPE_STRING_CHAIN);
    }

    if((a->type.state & FLAG_STRING_TEXT) != 0){
        if(!TextCharFilter(chars, length)){
            return ERROR;
        }
    }

    size_t l = length;
    size_t remaining = l;
    size_t copy_l = remaining;
    byte *bytes = (byte *)chars;

    String *seg = a;
    printf("Starting Length:%d %s\n", a->length, chars);
    byte *p = bytes;

    if(a->type.of == TYPE_STRING_CHAIN){
        while(seg->next != NULL){
            seg = seg->next;
        }
        if(seg->length == TYPE_STRING_CHAIN){
            String *next = String_Init(m, -1);
            seg->next = next;
            seg = next;
        }
    }else if(a->type.of == TYPE_STRING_FIXED){
        if(a->length+length > STRING_FIXED_SIZE){
            Fatal("length exceeds fixed size\n", a->type.of);
        }
    }else if(a->type.of == TYPE_STRING_FULL){
        if(a->length+length > STRING_FULL_SIZE){
            Fatal("length exceeds full size\n", a->type.of);
        }
    }else{
        Fatal("unknown type\n", a->type.of);
    }

    printf("Copying:\n");
    while(remaining > 0){
        copy_l = min(remaining, STRING_CHUNK_SIZE-seg->length); 
        printf("  l:%d -> %ld/%ld\n", seg->length, copy_l, remaining);
        memcpy(seg->bytes+seg->length, p, copy_l);
        printf("  coping...\n");
        seg->length += copy_l;
        if(seg->length == STRING_CHUNK_SIZE){
            printf("  new chunk...\n");
            String *next = String_Init(m, -1);
            seg->next = next;
            seg = seg->next;
        }
        remaining -= copy_l;
        p += copy_l;
    }

    if((a->type.state & DEBUG) != 0){
        printf("Length: %d\n", a->length);
    }

    return SUCCESS;
}

i64 String_Length(String *s) {
    String *tail = s;
    i64 length = 0;
    int i = 0;
    while(tail != NULL){
        length += tail->length; 
        tail = String_Next(tail);
        i++;
    }
    return length;
}

boolean String_EqualsBytes(String *a, byte *cstr){
    int l = strlen((char *)cstr); 
    int pos = 0;
    if(String_Length(a) != l){
        return FALSE;
    }
    String *tail = a;
    byte *p = cstr;
    while(tail != NULL && (pos+tail->length) <= l){
        if(strncmp((char *)tail->bytes, (char *)p, tail->length) != 0){
            return FALSE;
        }
        p += tail->length;
        pos += tail->length;
        tail = String_Next(tail);
    }

    if(tail == NULL && pos == l){
        return TRUE;
    }

    return FALSE;
}

boolean String_Eq(Abstract *a, void *b){
    if(a->type.of != TYPE_STRING_CHAIN && a->type.of != TYPE_STRING_FIXED
        || ((Abstract *)b)->type.of != TYPE_STRING_CHAIN && ((Abstract *)b)->type.of != TYPE_STRING_FIXED
    ){
       Fatal("", TYPE_STRING_CHAIN); 
    }
    String *sa = (String *)a;
    String *sb = (String *)b;
    return String_Equals(sa, sb);
}

boolean String_Equals(String *a, String *b){
    if(a == NULL){
        Fatal("String a is NULL", TYPE_STRING_CHAIN);
    }
    if(b == NULL){
        Fatal("String b is NULL", TYPE_STRING_CHAIN);
    }
    if(String_Length(a) != String_Length(b)){
        return FALSE;
    }
    String *aTail = a;
    String *bTail = b;
    while(aTail != NULL && bTail != NULL){
        if(aTail->length != bTail->length){
            return FALSE;
        }
        if(strncmp((char *)aTail->bytes, (char *)bTail->bytes, aTail->length) != 0){
            return FALSE;
        }
        if(aTail->type.of == TYPE_STRING_CHAIN){
            aTail = aTail->next;
        }else{
            aTail = NULL;
        }

        if(bTail->type.of == TYPE_STRING_CHAIN){
            bTail = bTail->next;
        }else{
            bTail = NULL;
        }
    }

    if(aTail == NULL && bTail == NULL){
        return TRUE;
    }

    return FALSE;
}

status String_Reset(String *s){
    status r = NOOP;
    while(s != NULL){
        if(s->length > 0){
            memset(s->bytes, 0, s->length);
            r |= SUCCESS;
        }
        s->length = 0;
        s = String_Next(s);
    }
    return r;
}


String *String_Next(String *s){
    if(s->type.of == TYPE_STRING_CHAIN){
        if((s->type.state & LOCAL_PTR) != 0 && s->m != NULL){
            return (String *)MemLocal_GetPtr(s->m, (LocalPtr *)s->next);
        }
        return s->next;
    }
    return NULL;
}
