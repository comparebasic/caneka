#include <external.h>
#include <caneka.h>

i64 Stream_ToB64(Stream *sm, byte *b, i32 length){
    return 0;
}

i64 Stream_ToStrVec(Stream *sm, byte *b, i32 length){
    i32 sz = STR_DEFAULT;
    i64 taken = 0;
    i64 offset = 0;
    i64 total = 0;
    if(sm->fd >= 0){
        return (i64)write(sm->fd, b, length);
    }else{
        while(length > 0 ){
            Str *s = sm->dest.it->value;
            if(s == NULL || s->length == s->alloc){
                sm->dest.it->type.state = 
                    (sm->dest.it->type.state & NORMAL_FLAGS) | SPAN_OP_ADD;
                s = sm->dest.it->value = Str_Make(sm->m, sz);
                Iter_Query(sm->dest.it);
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

