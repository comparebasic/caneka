#include <external.h>
#include <caneka.h>

i64 Stream_ToB64(Stream *sm, byte *b, i32 length){
    return 0;
}

i64 Stream_ToStrVec(Stream *sm, byte *b, i32 length){
    i32 sz = (sm->type.state & STREAM_512_BYTES) ? 512 : 128;
    i64 taken = 0;
    i64 offset = 0;
    i64 total = 0;
    if(sm->fd >= 0){
        return (i64)write(sm->fd, b, length);
    }else{
        while(length > 0 ){
            Str *s = sm->it.value;
            if(s == NULL || s->length == s->alloc){
                sm->it.type.state = 
                    (sm->it.type.state & NORMAL_FLAGS) | SPAN_OP_ADD;
                s = sm->it.value = Str_Make(sm->m, sz);
                Iter_Query(&sm->it);
                offset = 0;
            }
            taken = Str_Add(s, b+offset, length);
            total += taken;
            length -= taken;
            offset += length;
        }
    }
    return total;
}

