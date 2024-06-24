#include <external.h>
#include <filestore.h>
static const char *digits = "0123456789";

static String *string_Init(MemCtx *m){
    return (String *)MemCtx_Alloc(m, sizeof(String));
}

String *String_FromInt(MemCtx *m, int i){
    char buff[MAX_BASE10+1];
    memset(buff, 0, MAX_BASE10+1);

    i64 n = 0;
    i64 base = 10;
    int pows = 0;
    int position = MAX_BASE10-1;
    int val;
    char digit = digits[0];
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
    String *s = string_Init(m); 
    String_AddCstr(m, s, (char *)bytes, strlen((char *)bytes));

    return s;
}

String *String_MakeFixed(MemCtx *m, byte *bytes, int length){
    String *s = string_Init(m); 
    String_AddCstr(m, s, (char *)bytes, length);

    return s;
}

String *String_From(MemCtx *m, char *cstr){
    return String_Make(m, (byte *)cstr);
}

status String_Add(MemCtx *m, String *a, String *b) {
    return String_AddCstr(m, a, (char *)b->bytes, b->length);
}

status String_AddCstr(MemCtx *m, String *a, char *chars, int length) {
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
    if(copy_l > (STRING_CHUNK_SIZE - seg->length)){
        copy_l = (STRING_CHUNK_SIZE - seg->length);
    }

    if(seg->bytes == NULL){
        seg->bytes = (byte *)MemCtx_Alloc(m, copy_l+1);
        memcpy(seg->bytes, p, copy_l);
        seg->length = copy_l;
        remaining -= copy_l;
        p += copy_l;
    }else if(seg->length < STRING_CHUNK_SIZE){
        seg->bytes = MemCtx_Realloc(m, STRING_CHUNK_SIZE+1, (void *)seg->bytes, seg->length+1);
        memcpy(seg->bytes+seg->length, p, copy_l);
        seg->length += copy_l;
        remaining -= copy_l;
        p += copy_l;
    }

    /* if more than a string seg remains, make a new one */
    while(remaining > STRING_CHUNK_SIZE){
        String *next = string_Init(m);
        next->bytes = (byte *)MemCtx_Alloc(m, STRING_CHUNK_SIZE+1);
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
        String *next = string_Init(m);
        next->bytes = (byte *)MemCtx_Alloc(m, remaining+1);
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

status String_EqualsCStr(String *a, char *cstr){
    int l = strlen(cstr); 
    int pos = 0;
    if(String_Length(a) != l){
        return FALSE;
    }
    String *tail = a;
    char *p = cstr;
    while(tail != NULL && (pos+tail->length) <= l){
        if(strncmp((char *)tail->bytes, p, tail->length) != 0){
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
    String *s = string_Init(m);
    String *seg = range->start.seg;
    i64 remaining = range->length;

    i64 length = min(remaining, (seg->length - range->start.localPosition));
    char *p = (char *)seg->bytes+range->start.localPosition;
    String_AddCstr(m, s, p, length);
    remaining -= length;

    if(remaining > 0){
        while(seg != NULL){
            if(seg == range->end.seg){
                String_AddCstr(m, s, (char *)seg->bytes, range->end.localPosition);
                break;
            }else{
                String_Add(m, s, seg);
            }
            seg = seg->next;
        }
    }
    
    return s;
}
