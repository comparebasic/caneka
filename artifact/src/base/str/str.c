#include <external.h>
#include <caneka.h>

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
    i64 len = min(s->alloc - s->length, length);
    if(len > 0){
        memcpy(s->bytes+s->length, b, len);
    }
    if(length != len){
        s->type.state |= ERROR;
    }
    return len;
}

i64 Str_AddCstr(Str *s, char *cstr){
    i64 len = strlen(cstr);
    return Str_Add(s, (byte *)cstr, len);
}

char *Str_Cstr(MemCtx *m, Str *s){
    if(TextCharFilter(s->bytes, s->length)){
        byte *b;
        if(s->alloc == s->length+1){
            b = s->bytes;
        }else{
            b = MemCtx_Alloc(m, s->length+1);
            memcpy(b, s->bytes, s->length);
        }
        return (char *)b;
    }
    return NULL;
}

Str *Str_Clone(MemCtx *m, Str *s, word alloc){
    if(alloc < s->length){
        return NULL;
    }
    Str *ret = MemCtx_Alloc(m, sizeof(Str));
    byte *_bytes = MemCtx_Alloc(m, alloc);
    memcpy(_bytes, s->bytes, s->length);
    Str_Init(ret, _bytes, s->length, alloc);
    return ret;
}

Str *Str_From(MemCtx *m, byte *bytes, word length){
    Str *s = MemCtx_Alloc(m, sizeof(Str));
    byte *_bytes = MemCtx_Alloc(m, length);
    memcpy(_bytes, bytes, length);
    Str_Init(s, _bytes, length, length);
    return s;
}

Str *Str_Ref(MemCtx *m, byte *bytes, word length, word alloc){
    Str *s = MemCtx_Alloc(m, sizeof(Str));
    Str_Init(s, bytes, length, alloc);
    return s;
}

Str *Str_CstrRef(MemCtx *m, char *cstr){
    i64 len = strlen(cstr);
    Str *s = MemCtx_Alloc(m, sizeof(Str));
    Str_Init(s, (byte *)cstr, len, len+1);
    return s;
}

status Str_Init(Str *s, byte *bytes, word length, word alloc){
    s->type.of = TYPE_STR;
    s->bytes = bytes;
    s->length = length;
    s->alloc = alloc;
    return SUCCESS;
}

Str *Str_Make(MemCtx *m, word alloc){
    Str *s = MemCtx_Alloc(m, sizeof(Str));
    byte *bytes = MemCtx_Alloc(m, alloc);
    Str_Init(s, bytes, 0, alloc);
    return s;
}
