#include <external.h>
#include <caneka.h>

i64 Stream_To(Stream *sm, byte *b, i32 length){
    return sm->func(sm, b, length);
}

Stream *Stream_Make(MemCh *m, i32 fd, word flags, StreamFunc func){
    Stream *sm = (Stream *)MemCh_Alloc(m, sizeof(Stream));
    sm->type.of = TYPE_STREAM;
    sm->m = m;
    sm->func = func;
    if((flags & (STREAM_128_BYTES|STREAM_512_BYTES)) == 0){
        flags |= STREAM_128_BYTES;
    }
    sm->type.state = flags;
    if(fd < 0){
        sm->v = StrVec_Make(m);
    }
    sm->fd = fd;
    Iter_Init(&sm->it, sm->v->p);
    return sm;
}

