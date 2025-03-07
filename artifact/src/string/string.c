#include <external.h>
#include <caneka.h>

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

i64 String_GetSegSize(String *s){
    asIfc(s, TYPE_STRING);
    if(s->type.of == TYPE_STRING_CHAIN){
        return STRING_CHUNK_SIZE;
    }else if(s->type.of == TYPE_STRING_FIXED){
        return STRING_FIXED_SIZE;
    }else if(s->type.of == TYPE_STRING_FULL){
        return STRING_FULL_SIZE;
    }
    return 0;
}

String *String_Clone(MemCtx *m, String *s){
    String *s2 = String_Init(m, s->length);
    String_Add(m, s2, s);
    return s2;
}

status String_Trunc(String *s, i64 len){
    DebugStack_Push("String_Trunc", TYPE_CSTR);
    if(len < 0){
        len = String_Length(s) + len;
    }
    i64 actual = 0;
    String *tail = s;
    size_t sz = String_GetSegSize(s);
    while(tail != NULL){
        if(actual+tail->length > len){
            tail->length = len - actual;
            memset(tail->bytes+tail->length, 0, sz-tail->length);
            if(String_Next(s) != NULL){
                tail->next = NULL;
            }
            DebugStack_Pop();
            return SUCCESS;
        }
        actual += tail->length; 
        tail = String_Next(s); 
    };

    DebugStack_Pop();
    return NOOP;
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

status String_AddBytes(MemCtx *m, String *a, byte *chars, int length) {
    DebugStack_Push(a, a->type.of);
    if(length == 0){
       DebugStack_Pop();
       return NOOP; 
    }
    if(a == NULL){
        Fatal("Error string is NULL", TYPE_STRING_CHAIN);
    }

    if((a->type.state & FLAG_STRING_TEXT) != 0){
        if(!TextCharFilter(chars, length)){
            DebugStack_Pop();
            return ERROR;
        }
    }

    size_t remaining = length;
    size_t copy_l = remaining;
    byte *bytes = (byte *)chars;

    String *seg = a;
    if((a->type.state & DEBUG) != 0){
        printf("\nCopying, Starting Length:\n    %d New Chars:%d/'%s'\n", a->length, length, chars);
    }
    byte *p = bytes;

    if(isDebug(a)){
        printf("I\n");
    }
    if(a->type.of == TYPE_STRING_CHAIN){
        while(seg->next != NULL){
            if((a->type.state & DEBUG) != 0){
                printf("  > next\n");
            }
            seg = seg->next;
            if(seg->length == 0){
                break;
            }
        }
        if((a->type.state & DEBUG) != 0){
            printf("  Seg len %d\n", seg->length);
        }
        if(seg->length == STRING_CHUNK_SIZE){
            seg->next = String_Init(m, -1);
            seg = seg->next;
        }
        if(isDebug(a)){
            printf("II chain\n");
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

    while(remaining > 0){
        if((a->type.state & FLAG_STRING_CONTIGUOUS) != 0){
            if(STRING_CHUNK_SIZE-seg->length < remaining){
                seg->next = String_Init(m, -1);
                seg = seg->next;
            }
            copy_l = remaining; 
        }else{
            copy_l = min(remaining, STRING_CHUNK_SIZE-seg->length); 
        }
        if((a->type.state & DEBUG) != 0){
            printf("  l:%d -> +%ld remaining %ld\n", seg->length, copy_l, remaining);
        }
        memcpy(seg->bytes+seg->length, p, copy_l);
        seg->length += copy_l;
        if(seg->length == STRING_CHUNK_SIZE){
            if((a->type.state & DEBUG) != 0){
                printf("  > new seg\n");
            }
            seg->next = String_Init(m, -1);
            if(seg->next == NULL ){
                seg->next = String_Init(m, -1);
            }
            seg = seg->next;
        }
        remaining -= copy_l;
        p += copy_l;
    }

    if((a->type.state & DEBUG) != 0){
        printf("  Current(%ld): \n", String_Length(a));
        String *tail = a;
        while(tail != NULL){
            printf("    %d:'%s'\n", tail->length, tail->bytes);
            tail = String_Next(tail);
        }
        printf("\n");
    }

    DebugStack_Pop();
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

String *String_Next(String *s){
    if(s->type.of == TYPE_STRING_CHAIN){
        return s->next;
    }
    return NULL;
}

String *String_Make(MemCtx *m, byte *bytes){
    DebugStack_Push("String_Make", TYPE_CSTR);
    int length = strlen((char *)bytes);
    String *s = String_Init(m, length); 
    String_AddBytes(m, s, bytes, length);
    DebugStack_Pop();
    return s;
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
    if(m->type.range == -1){
        s->type.state |= LOCAL_PTR;
    }

    return s;
}

