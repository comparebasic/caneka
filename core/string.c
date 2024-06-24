#include <external.h>
#include <filestore.h>

static const byte *digits = (byte *)"0123456789";

String *String_Init(MemCtx *m, int expected){
    size_t sz = sizeof(StringMin);
    cls type = TYPE_STRING_FIXED;
    if(expected < 0 || expected >= STRING_FIXED_SIZE){
        sz = sizeof(String);
        type = TYPE_STRING_CHAIN;
    }
    String *s =  (String *)MemCtx_Alloc(m, sz);
    s->type.of = type;
    return s;
}

String *String_FromInt(MemCtx *m, int i){
    byte buff[MAX_BASE10+1];
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

    return String_From(m, buff+position+1); 
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
    return String_AddBytes(m, a, b->bytes, b->length);
}

status String_AddBytes(MemCtx *m, String *a, byte *chars, int length) {

    size_t l = length;
    size_t remaining = l;
    size_t copy_l = remaining;
    byte *bytes = (byte *)chars;

    String *seg = a;
    String *tail = seg;
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
            seg = tail = next;
        }else{
            tail->next = next;
            tail = next;
        }
        remaining -= STRING_CHUNK_SIZE;
    }

    /* if any remains, make a final seg */
    if(remaining > 0){
        String *next = String_Init(m, -1);
        memcpy(next->bytes, p, remaining);
        next->length = remaining;

        if(seg == NULL){
            seg = tail = next;
        }else{
            tail->next = next;
            tail = next;
        }
    }

    return SUCCESS;
}

i64 String_Length(String *s) {
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

status String_EqualsBytes(String *a, byte *cstr){
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

status String_Equals(String *a, String *b){
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
        aTail = aTail->next;
        bTail = bTail->next;
    }

    if(aTail == NULL && bTail == NULL){
        return TRUE;
    }

    return FALSE;
}

String *String_FromRange(MemCtx *m, Range *range){
    if(range->state != COMPLETE){
        return NULL;
    }
    String *seg = range->start.seg;
    i64 remaining = range->length;
    String *s = String_Init(m, remaining);

    i64 length = min(remaining, (seg->length - range->start.localPosition));
    byte *p = seg->bytes+range->start.localPosition;
    String_AddBytes(m, s, p, length);
    remaining -= length;

    if(remaining > 0){
        while(seg != NULL){
            if(seg == range->end.seg){
                String_AddBytes(m, s, seg->bytes, range->end.localPosition);
                break;
            }else{
                String_Add(m, s, seg);
            }
            seg = seg->next;
        }
    }
    
    return s;
}
