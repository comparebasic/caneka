#include <external.h>
#include <caneka.h>

Str *Str_FromI64(MemCtx *m, i64 i){
    Str *s = Str_Make(m, MAX_BASE10);
    byte *end = s->bytes+(MAX_BASE10-1);
    byte *b = end;

    i32 base = 10;
    i64 val;
    boolean negative = i < 0;
    if(negative){
        i = labs(i);
    }
    *b = '0';
    while(i > 0){
        val = i % base;
        *(b--) = '0'+val;
        i -= val;
        i /= base;
    }

    if(negative){
        *(b--) = '-';
    }

    i64 length = end - b;
    s->alloc = length;
    s->length = length;
    s->bytes = b+1;
    return s;
}

i64 Str_ToFd(Str *s, int fd){
    return write(fd, s->bytes, s->length);
}

Str *Str_From(MemCtx *m, byte *bytes, word length){
    Str *s = MemCtx_Alloc(m, sizeof(Str));
    word alloc = length;
    word rmd = length % sizeof(void *);
    if(rmd){
        alloc += sizeof(void *)-rmd;
    }
    byte *_bytes = MemCtx_Alloc(m, alloc);
    memcpy(_bytes, bytes, length);
    Str_Init(s, _bytes, length, alloc);
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
