#include <external.h>
#include <caneka.h>

i64 Str_ToFd(Str *s, int fd){
    return write(fd, s->bytes, s->length);
}

i64 Str_Add(Str *s, byte *b, i64 length){
    i64 len = min(s->alloc - s->length, length);
    if(len > 0){
        memcpy(s->bytes+s->length, b, len);
    }
    return len;
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
