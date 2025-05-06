#include <external.h>
#include <caneka.h>

i64 Stream_ToB64(Stream *sm, byte *b, i32 length){
    return 0;
}

StrVec *StrVec_FromBytes(MemCh *m, byte *b, i32 length){
    Stream sm;
    memset(&sm, 0, sizeof(Stream));
    StrVec *v = StrVec_Make(m);
    Stream_SetupMakeStrVec(m, &sm, v);
    Stream_Bytes(&sm, b, length);
    return v;
}

i64 Stream_ToFd(Stream *sm, byte *b, i32 length){
    return write(sm->fd, b, (size_t)length);
}

i64 Stream_ToStrVec(Stream *sm, byte *b, i32 length){
    i32 sz = STR_DEFAULT;
    i64 taken = 0;
    i64 offset = 0;
    i64 total = 0;
    Cursor *curs = sm->dest.curs;
    StrVec *v = sm->dest.curs->v;
    while(length > 0 ){
        Str *s = curs->it.value;
        if(s == NULL || s->length == s->alloc){
            curs->it.type.state = 
                (curs->it.type.state & NORMAL_FLAGS) | SPAN_OP_ADD;
            s = curs->it.value = Str_Make(sm->m, sz);
            Iter_Query(&curs->it);
        }
        taken = Str_Add(s, b+offset, length);
        v->total += taken;
        total += taken;
        length -= taken;
        offset += taken;
    }
    return total;
}

