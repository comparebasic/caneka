#include <external.h>
#include <caneka.h>

i64 Stream_To(Stream *sm, byte *b, i32 length){
    return sm->func(sm, b, length);
}

i64 Stream_Read(Stream *sm, i32 length){
    if(sm->type.of & STREAM_FROM_FD){
        if(sm->type.of & STREAM_SOCKET){
            /* read using recv */
        }else{
            /* read using read */
        }
    }
    return 0;
}

Stream *Stream_MakeStrVec(MemCh *m){
    Stream *sm = Stream_Make(m);
    sm->type.state |= STREAM_STRVEC;
    StrVec *v = StrVec_Make(m);
    Cursor_Setup(sm->dest.curs, v);
    return sm;
}

Stream *Stream_MakeChain(MemCh *m, Span *chain){
    Stream *sm = Stream_Make(m);
    sm->type.state |= STREAM_CHAIN;
    sm->dest.it = Iter_Make(m, chain);
    return sm;
}

Stream *Stream_MakeFromFd(MemCh *m, i32 fd, word flags){
    Stream *sm = Stream_Make(m);
    sm->type.state |= (STREAM_FROM_FD|flags);
    sm->fd = fd;
    StrVec *v = StrVec_Make(m);
    Cursor_Setup(sm->dest.curs, v);
    return sm;
}

Stream *Stream_MakeToFd(MemCh *m, i32 fd, StrVec *v, word flags){
    Stream *sm = Stream_Make(m);
    sm->type.state |= (STREAM_TO_FD|flags);
    sm->fd = fd;
    Cursor_Setup(sm->dest.curs, v);
    return sm;
}

Stream *Stream_Make(MemCh *m){
    Stream *sm = (Stream *)MemCh_Alloc(m, sizeof(Stream));
    sm->type.of = TYPE_STREAM;
    sm->m = m;
    return sm;
}
