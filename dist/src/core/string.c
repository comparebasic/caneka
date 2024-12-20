#include <external.h>
#include <caneka.h>

size_t String_GetSegSize(String *s){
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

status String_Trunc(String *s, i64 len){
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
            return SUCCESS;
        }
        actual += tail->length; 
        tail = String_Next(s); 
    };

    return NOOP;
}

String *String_Clone(MemCtx *m, String *s){
    String *s2 = String_Init(m, s->length);
    String_Add(m, s2, s);
    return s2;
}

String *String_MakeFixed(MemCtx *m, byte *bytes, int length){
    String *s = String_Init(m, length); 
    String_AddBytes(m, s, bytes, length);

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

status String_AddBytes(MemCtx *m, String *a, byte *chars, int length) {
    if(a == NULL){
        Fatal("Error string is NULL", TYPE_STRING_CHAIN);
    }

    if((a->type.state & FLAG_STRING_TEXT) != 0){
        if(!TextCharFilter(chars, length)){
            return ERROR;
        }
    }

    size_t remaining = length;
    size_t copy_l = remaining;
    byte *bytes = (byte *)chars;

    String *seg = a;
    if((a->type.state & DEBUG) != 0){
        printf("\nCopying, Starting Length:\n    %d New Chars:'%s'\n", a->length, chars);
    }
    byte *p = bytes;

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
        copy_l = min(remaining, STRING_CHUNK_SIZE-seg->length); 
        if((a->type.state & DEBUG) != 0){
            printf("  l:%d -> %ld/%ld\n", seg->length, copy_l, remaining);
        }
        memcpy(seg->bytes+seg->length, p, copy_l);
        seg->length += copy_l;
        if(seg->length == STRING_CHUNK_SIZE){
            if((a->type.state & DEBUG) != 0){
                printf("  > new seg\n");
            }
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

String *String_Make(MemCtx *m, byte *bytes){
    int length = strlen((char *)bytes);
    String *s = String_Init(m, length); 
    String_AddBytes(m, s, bytes, length);
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
    s->type.state |= (m->type.state & LOCAL_PTR);

    return s;
}

