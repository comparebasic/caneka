#include <external.h>
#include <caneka.h>

boolean TextCharFilter(byte *b, i64 length){
    byte *end = b+(length-1);
    while(TRUE){
        byte _b = *b;
        if(_b < 32 && ( _b != '\r' && _b != '\n' && _b != '\t')){
            return FALSE;
        }
        if(b == end){
            break;
        }
        b++;
    }
    return TRUE;
}

status Str_Reset(Str *s){
    s->length = 0;
    return SUCCESS;
}

status Str_Wipe(Str *s){
    memset(s->bytes, 0, s->length);
    return SUCCESS;
}

i64 Str_ToFd(Str *s, int fd){
    return write(fd, s->bytes, s->length);
}

i64 Str_Add(Str *s, byte *b, i64 length){
    i64 len = length;
    i64 remaining = (i64)s->alloc - s->length;
    if(length > remaining){
       len = remaining; 
       s->type.state |= ERROR;
    }
    if(len > 0){
        memcpy(s->bytes+s->length, b, len);
        s->length += len;
        return len;
    }else{
        return 0;
    }
}

i64 Str_AddCstr(Str *s, char *cstr){
    i64 len = strlen(cstr);
    return Str_Add(s, (byte *)cstr, len);
}

char *Str_Cstr(MemCh *m, Str *s){
    if(TextCharFilter(s->bytes, s->length)){
        byte *b;
        if(s->alloc == s->length+1){
            b = s->bytes;
        }else{
            b = MemCh_Alloc(m, s->length+1);
            memcpy(b, s->bytes, s->length);
        }
        return (char *)b;
    }
    return NULL;
}

Str *Str_Clone(MemCh *m, Str *s, word alloc){
    if(alloc < s->length){
        return NULL;
    }
    Str *ret = MemCh_Alloc(m, sizeof(Str));
    byte *_bytes = MemCh_Alloc(m, alloc);
    memcpy(_bytes, s->bytes, s->length);
    Str_Init(ret, _bytes, s->length, alloc);
    return ret;
}

Str *Str_From(MemCh *m, byte *bytes, word length){
    Str *s = MemCh_Alloc(m, sizeof(Str));
    byte *_bytes = MemCh_Alloc(m, length);
    memcpy(_bytes, bytes, length);
    Str_Init(s, _bytes, length, length);
    return s;
}

Str *Str_Ref(MemCh *m, byte *bytes, word length, word alloc){
    Str *s = MemCh_Alloc(m, sizeof(Str));
    Str_Init(s, bytes, length, alloc);
    s->type.state |= STRING_CONST;
    return s;
}

Str *Str_CstrRef(MemCh *m, char *cstr){
    i64 len = strlen(cstr);
    Str *s = MemCh_Alloc(m, sizeof(Str));
    Str_Init(s, (byte *)cstr, len, len+1);
    s->type.state |= STRING_CONST;
    return s;
}

status Str_Init(Str *s, byte *bytes, word length, word alloc){
    s->type.of = TYPE_STR;
    s->bytes = bytes;
    s->length = length;
    s->alloc = alloc;
    return SUCCESS;
}

Str *Str_Make(MemCh *m, word alloc){
    Str *s = MemCh_Alloc(m, sizeof(Str));
    byte *bytes = MemCh_Alloc(m, alloc);
    Str_Init(s, bytes, 0, alloc);
    return s;
}
