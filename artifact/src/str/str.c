
Str *Str_From(MemCtx *m, byte *bytes, word length){
    Str *s = MemCtx_Alloc(m, sizeof(Str));
    word alloc = length;
    word rmd = length % sizeof(slot);
    if(rmd){
        alloc += sizeof(slot)-rmd;
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
