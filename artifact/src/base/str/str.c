#include <external.h>
#include <caneka.h>

boolean TextCharFilter(byte *b, i64 length){
    byte *end = b+(length-1);
    while(TRUE){
        if(*b < 32 && ( *b != '\r' && *b != '\n' && *b != '\t')){
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

status Str_Decr(Str *s, word length){
    if(length > s->length){
        s->type.state |= ERROR;
        return s->type.state;
    }else{
        s->length -= length;
        return s->type.state;
    }
}

status Str_Incr(Str *s, word length){
    if(s->length == length){
        s->length = 0;
        s->type.state |= SUCCESS;
    }else if(s->length > length){
        s->bytes += length;
        s->length -= length;
    }else{
        s->length = 0;
        s->type.state |= ERROR;
    }
    return s->type.state;
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
            b = Bytes_Alloc(m, s->length+1, TYPE_BYTES_POINTER);
            memcpy(b, s->bytes, s->length);
        }
        return (char *)b;
    }
    return NULL;
}

Str *Str_Clone(MemCh *m, Str *s){
    return Str_CloneAlloc(m, s, s->alloc);
}

Str *Str_Rec(MemCh *m, Str *s){
    Str *dup = (Str *)MemCh_AllocOf(m, sizeof(Str), TYPE_STR);
    memcpy(dup, s, sizeof(Str));
    return dup;
}

Str *Str_CloneAlloc(MemCh *m, Str *s, word alloc){
    Str *ret = MemCh_Alloc(m, sizeof(Str));
    byte *_bytes = Bytes_Alloc(m, (size_t)alloc, TYPE_BYTES_POINTER);
    memcpy(_bytes, s->bytes, min(s->length, alloc));
    Str_Init(ret, _bytes, s->length, alloc);
    return ret;
}

Str *Str_From(MemCh *m, byte *bytes, word length){
    Str *s = MemCh_Alloc(m, sizeof(Str));
    byte *_bytes = Bytes_Alloc(m, length, TYPE_BYTES_POINTER);
    memcpy(_bytes, bytes, length);
    Str_Init(s, _bytes, length, length);
    return s;
}

Str *Str_Ref(MemCh *m, byte *bytes, word length, word alloc, word flags){
    if(length < 0 || length > STR_MAX){
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Error cannot have a negative length of a string at end",
        NULL);
        return NULL;
    }

    Str *s = MemCh_Alloc(m, sizeof(Str));
    Str_Init(s, bytes, length, alloc);
    s->type.state = flags;
    if(flags & STRING_COPY){
        byte *_bytes = Bytes_Alloc(m, alloc, TYPE_BYTES_POINTER);
        memcpy(_bytes, s->bytes, s->length);
        s->bytes = _bytes;
    }
    return s;
}

Str *Str_FromCstr(MemCh *m, char *cstr, word flags){
    i64 len = strlen(cstr);
    if((len+1) > STR_MAX){
        Abstract *args[] = {
            (Abstract *)I64_Wrapped(m, len),
            NULL
        };
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "strlen too long to make Str, have $", args);
        return NULL;
    }
    Str *s = MemCh_Alloc(m, sizeof(Str));
    s->type.state = flags;
    byte *bytes = (byte *)cstr;
    if(flags & STRING_COPY){
        bytes = Bytes_Alloc(m, len+1, TYPE_BYTES_POINTER);
        memcpy(bytes, cstr, len);
    }
    Str_Init(s, bytes, len, len+1);
    s->type.state |= STRING_CONST;
    return s;
}

Str *Str_CstrRef(MemCh *m, char *cstr){
    return Str_FromCstr(m, cstr, ZERO);
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
    byte *bytes = Bytes_Alloc(m, alloc, TYPE_BYTES_POINTER);
    Str_Init(s, bytes, 0, alloc);
    return s;
}
