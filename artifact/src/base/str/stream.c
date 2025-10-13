#include <external.h>
#include <caneka.h>

i64 Stream_IndentOut(Stream *sm){
    i64 total = 0;
    if(sm->indent > 0){
        i32 indent = sm->indent;
        while(indent--){
            total += Stream_Bytes(sm, (byte *)"  ", 2);
        }
    }
    return total;
}

StreamTask *StreamTask_Make(MemCh *m, Stream *sm, Abstract *a, StreamAbsFunc func){
    StreamTask *tsk = (StreamTask *)MemCh_Alloc(m, sizeof(StreamTask));
    tsk->type.of = TYPE_STREAM_TASK;
    tsk->sm = sm;
    tsk->a = a;
    tsk->func = func;
    return tsk;
}

status Stream_Move(Stream *sm, i32 offset){
    if((sm->type.state & STREAM_STRVEC) && (sm->type.state & STREAM_FROM_FD) == 0){
        if(offset == 0){
            return NOOP;
        }else if(offset < 0){
            offset = abs(offset);
            if(sm->dest.curs->type.state & END && offset > 0){
               offset--; 
            }
            return Cursor_Decr(sm->dest.curs, offset);
        }else{
            sm->type.state |=  (Cursor_Incr(sm->dest.curs, offset) & END);
            return sm->type.state;
        }
    }else{
        Error(ErrStream->m, (Abstract *)sm, FUNCNAME, FILENAME, LINENUMBER,
            "Not implemented", NULL);
        return ERROR;
    }
}

status Stream_SeekEnd(Stream *sm, i32 offset){
    if((sm->type.state & STREAM_STRVEC) && (sm->type.state & STREAM_FROM_FD) == 0){
        Cursor_End(sm->dest.curs);
        if(offset){
            return Stream_Move(sm, -offset);
        }else{
            return SUCCESS;
        }
    }else{
        return lseek(sm->fd, offset, SEEK_END) != -1 ? SUCCESS : ERROR; 
        Error(ErrStream->m, (Abstract *)sm, FUNCNAME, FILENAME, LINENUMBER,
            "Not implemented", NULL);
        return ERROR;
    }
}

status Stream_Seek(Stream *sm, i32 offset){
    if((sm->type.state & STREAM_STRVEC) && (sm->type.state & STREAM_FROM_FD) == 0){
        Cursor_Setup(sm->dest.curs, sm->dest.curs->v);
        return Stream_Move(sm, offset);
    }else{
        return lseek(sm->fd, offset, SEEK_SET) != -1 ? SUCCESS : ERROR; 
    }
}

status Stream_RFillStr(Stream *sm, Str *s){
    if((sm->type.state & STREAM_STRVEC) && (sm->type.state & STREAM_FROM_FD) == 0){
        sm->type.state |= (Cursor_RFillStr(sm->dest.curs, s) & (SUCCESS|ERROR|END));
        return sm->type.state;
    }else{ 
        lseek(sm->fd, -s->length, SEEK_CUR); 
        if(read(sm->fd, s->bytes, s->length) == s->length){
            lseek(sm->fd, -s->length, SEEK_CUR); 
            return SUCCESS;
        }else{
            return ERROR;
        }
    }
}

status Stream_FillStr(Stream *sm, Str *s){
    if((sm->type.state & STREAM_STRVEC) && (sm->type.state & STREAM_FROM_FD) == 0){
        return Cursor_FillStr(sm->dest.curs, s);
    }else{ 
        i64 length = read(sm->fd, s->bytes, s->length);
        if(length == s->length){
            return SUCCESS;
        }else{
            printf("Did not read all %ld from %d\n", length, sm->fd);
            return ERROR;
        }
    }
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

i64 Stream_ReadToMem(Stream *sm, i32 length, byte *mem){
    if(sm->type.state & STREAM_FROM_FD){
        if(sm->type.of & STREAM_SOCKET){
            return recv(sm->fd, mem, length, 0);
        }else{
            i64 total = read(sm->fd, mem, length);
            if(total == 0 && (sm->type.state & STREAM_ASYNC) == 0){
                sm->type.state |= END;
                return length;
            }else{
                return total;
            }
        }
    }
    return -1;
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

Stream *Stream_MakeFromFd(MemCh *m, i32 fd, word flags){
    Stream *sm = Stream_Make(m);
    sm->type.state |= (STREAM_FROM_FD|flags);
    sm->fd = fd;
    StrVec *v = StrVec_Make(m);
    sm->dest.curs = Cursor_Make(m, v);
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
