#include <external.h>
#include <caneka.h>

boolean TextCharFilter(byte *b, i64 length){
    byte *end = b+length;
    while(b < end){
        byte c = *b;
        if(!IS_VISIBLE(c)){
            return FALSE;
        }
        b++;
    }
    return TRUE;
}

String *Cont(MemCtx *m, byte *bytes){
    int length = strlen((char *)bytes);
    String *s = String_Init(m, length); 
    s->type.state |= FLAG_STRING_TEXT;
    String_AddBytes(m, s, bytes, length);
    return s;
}

String *Buff(MemCtx *m, byte *content){
    String *s = String_Init(m, STRING_EXTEND);
    s->type.state |= FLAG_STRING_TEXT;
    if(content != NULL){
        String_AddBytes(m, s, content, strlen((char *)content));
    }
    return s;
}
