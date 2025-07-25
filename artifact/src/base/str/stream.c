#include <external.h>
#include <caneka.h>

StreamTask *StreamTask_Make(MemCh *m, Stream *sm, Abstract *a, StreamAbsFunc func){
    StreamTask *tsk = (StreamTask *)MemCh_Alloc(m, sizeof(StreamTask));
    tsk->type.of = TYPE_STREAM_TASK;
    tsk->sm = sm;
    tsk->a = a;
    tsk->func = func;
    return tsk;
}

i64 Stream_Bytes(Stream *sm, byte *b, i32 length){
    return sm->func(sm, b, length);
}

i64 Stream_VecTo(Stream *sm, StrVec *v){
    Iter it;
    Iter_Init(&it, v->p);
    i64 total = 0;
    while((Iter_Next(&it) & END) == 0){
        Str *s = (Str *)it.value;
        total += Stream_Bytes(sm, s->bytes, s->length);
    }
    return total;
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

status Stream_SetupMakeStrVec(MemCh *m, Stream *sm, StrVec *v){
    sm->type.of = TYPE_STREAM;
    sm->m = m;
    sm->type.state |= STREAM_STRVEC;
    sm->dest.curs = Cursor_Make(m, v);
    sm->func = Stream_ToStrVec;
    return SUCCESS;
}

Stream *Stream_MakeStrVec(MemCh *m){
    Stream *sm = Stream_Make(m);
    StrVec *v = StrVec_Make(m);
    Stream_SetupMakeStrVec(m, sm, v);
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
    sm->func = Stream_ToFd;
    if(v != NULL){
        sm->dest.curs = Cursor_Make(m, v);
    }
    return sm;
}

Stream *Stream_Make(MemCh *m){
    Stream *sm = (Stream *)MemCh_Alloc(m, sizeof(Stream));
    sm->type.of = TYPE_STREAM;
    sm->m = m;
    sm->fd = -1;
    return sm;
}
