#include <external.h>
#include <caneka.h>

static const byte *digits = (byte *)"0123456789";

static int _String_FromInt(MemCtx *m, int i, byte buff[]){
    memset(buff, 0, MAX_BASE10+1);

    i64 n = 0;
    i64 base = 10;
    int pows = 0;
    int position = MAX_BASE10-1;
    int val;
    byte digit = digits[0];
    while(i > 0){
        val = i % base;
        digit = digits[val];
        buff[position] = digit;
        i -= val;
        i /= 10;
        position--;
    }
    return position;
}


String *String_Init(MemCtx *m, int expected){
    size_t sz = sizeof(StringMin);
    cls type = TYPE_STRING_FIXED;
    if(expected < 0 || expected >= STRING_FIXED_SIZE){
        sz = sizeof(String);
        type = TYPE_STRING_CHAIN;
    }
    String *s = (String *)MemCtx_Alloc(m, sz);
    s->type.of = type;
    return s;
}

String *String_ToEscaped(MemCtx *m, String *s){
    int length = 0;
    byte i8;
    for(int i = 0; i < s->length; i++){
        i8 = s->bytes[i];
        if(i8 > 31 && i8 < 127){
            length++;
        }else{
            if(
                i8 == '\r' ||
                i8 == '\t' ||
                i8 == '\n' ||
                i8 == '\\' ||
                i8 < 10
            ){
                length += 2;
            }else if (i8 < 100){
                length += 3;
            }else{
                length += 4;
            }
        }
    }
    String *s2 = String_Init(m, length);
    int position;
    for(int i = 0; i < s->length; i++){
        i8 = s->bytes[i];
        byte buff[MAX_BASE10+1];
        if(i8 > 31 && i8 < 127){
            String_AddBytes(m, s2, &i8, 1);
            continue;
        }else{
            String_AddBytes(m, s2, bytes("\\"), 1);
            if(i8 == '\r'){
                String_AddBytes(m, s2, bytes("r"), 1);
            }else if(i8 == '\n'){
                String_AddBytes(m, s2, bytes("n"), 1);
            }else if(i8 == '\\'){
                String_AddBytes(m, s2, bytes("\\"), 1);
            }else if(i8 == '\t'){
                String_AddBytes(m, s2, bytes("t"), 1);
            }else{
                String_AddInt(m, s2, i8);
            }
        }
    }

    return s2;
}

String *String_FromInt(MemCtx *m, int i){
    byte buff[MAX_BASE10+1];
    int position = _String_FromInt(m, i, buff);
    return String_From(m, buff+position+1); 
}

status String_AddInt(MemCtx *m, String *s, int i){
    byte buff[MAX_BASE10+1];
    int position = _String_FromInt(m, i, buff);
    return String_AddBytes(m, s, buff+position+1, MAX_BASE10-position-1);
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

status String_Add(MemCtx *m, String *a, String *b) {
    status r = READY;
    while(b != NULL && r != ERROR){
        r = String_AddBytes(m, a, b->bytes, b->length);
        b = String_Next(b);
    }
    return r;
}

status String_AddBytes(MemCtx *m, String *a, byte *chars, int length) {
    if(a == NULL){
        Fatal("Error string is NULL", TYPE_STRING_CHAIN);
    }

    size_t l = length;
    size_t remaining = l;
    size_t copy_l = remaining;
    byte *bytes = (byte *)chars;

    String *seg = a;
    byte *p = bytes;

    while(seg->next != NULL){
        seg = seg->next;
    }

    /* copy the initial chunk */
    if(seg->type.of == TYPE_STRING_FIXED){
        copy_l = min((STRING_FIXED_SIZE - seg->length), remaining);
    }else{
        copy_l = min((STRING_CHUNK_SIZE - seg->length), remaining);
    }

    if(copy_l > remaining && a->type.of != TYPE_STRING_CHAIN){
        printf("Returing ERROR not a flexible string\n");
        return ERROR;
    }

    memcpy(seg->bytes+seg->length, p, copy_l);
    seg->length += copy_l;
    remaining -= copy_l;
    p += copy_l;

    /* if more than a string seg remains, make a new one */
    while(remaining > STRING_CHUNK_SIZE){
        String *next = String_Init(m, -1);
        memcpy(next->bytes, p, STRING_CHUNK_SIZE);
        next->length = STRING_CHUNK_SIZE;
        p += STRING_CHUNK_SIZE;

        if(seg == NULL){
            seg = next;
        }else{
            seg->next = next;
            seg = next;
        }
        remaining -= STRING_CHUNK_SIZE;
    }

    /* if any remains, make a final seg */
    if(remaining > 0){
        String *next = String_Init(m, -1);
        memcpy(next->bytes, p, remaining);
        next->length = remaining;

        if(seg == NULL){
            seg = next;
        }else{
            seg->next = next;
            seg = next;
        }
    }

    return SUCCESS;
}

i64 String_Length(String *s) {
    if(s->type.of == TYPE_STRING_FIXED){
        return s->length;
    }
    String *tail = s;
    i64 length = 0;
    int i = 0;
    while(tail != NULL){
        length += tail->length; 
        tail = tail->next;
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
        tail = tail->next;
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

String *String_Next(String *s){
    if(s->type.of == TYPE_STRING_CHAIN){
        return s->next;
    }
    return NULL;
}
