#include <external.h>
#include <caneka.h>

static void Buff_setUnsentStr(Buff *bf){
    Str *s = (Str *)Str_Rec(bf->m, Span_Get(bf->v->p, bf->unsent.idx));
    if(s != NULL){
        s = Str_Rec(bf->m, s);
    }
    bf->unsent.s = s; 
    bf->unsent.offset = 0;
}

static void Buff_setUnsentIncr(Buff *bf, word length){
    Str_Incr(bf->unsent.s, length);
    bf->unsent.offset += length;
}

static status Buff_vecPosFrom(Buff *bf, i32 offset, i64 whence){
    Abstract *args[5];
    if(whence == SEEK_END){
        if(offset < 0){
            args[0] = (Abstract *)bf;
            args[1] = (Abstract *)I32_Wrapped(bf->m, offset);
            args[2] = NULL;
            Error(bf->m, FUNCNAME, FILENAME, LINENUMBER,
                "Offset cannot be zero if seeking from the end $: $", args);
            bf->type.state |= ERROR;
            DebugStack_Pop();
            return bf->type.state;
        }
        bf->unsent.idx = bf->v->p->max_idx;
        if(offset == 0){
            bf->type.state |= END;
            bf->unsent.s = NULL;
            bf->unsent.total = 0;
            DebugStack_Pop();
            return bf->type.state;
        }else{
            bf->unsent.total = 0;
            offset = -offset;
            Buff_setUnsentStr(bf);
        }
    }else{
        if(bf->unsent.s == NULL){
            bf->unsent.idx = 0;
            Buff_setUnsentStr(bf);
            bf->unsent.total = bf->v->total;
        }

        if(whence == SEEK_SET){
            if(offset + bf->unsent.total > bf->v->total ||
                    offset + bf->unsent.total < 0){
                args[0] = (Abstract *)bf;
                args[1] = (Abstract *)I32_Wrapped(bf->m, offset);
                args[2] = (Abstract *)I32_Wrapped(bf->m, bf->v->total);
                args[3] = NULL;
                Error(bf->m, FUNCNAME, FILENAME, LINENUMBER,
                    "Offset + unsent.total cannot be greater than buff->v->total $: $ + $",
                    args);

            }
            offset += bf->unsent.total - bf->v->total;
        }
    }

    if(offset > 0){
        while(bf->unsent.s->length < offset){
            offset -= bf->unsent.s->length;
            bf->unsent.total -= bf->unsent.s->length;
            bf->unsent.idx++;
            Buff_setUnsentStr(bf);
            if(bf->unsent.s == NULL){
                break;
            }
        }

        if(bf->unsent.s == NULL && offset > 0){
            bf->type.state |= ERROR;
            Error(bf->m, FUNCNAME, FILENAME, LINENUMBER,
                "Reached end of vec without using all of offset", NULL); 
            DebugStack_Pop();
            return bf->type.state;
        }

        if(offset > 0){
            Buff_setUnsentIncr(bf, offset);
            bf->unsent.total -= offset;
        }

    }else{
        offset = abs(offset);
        if(bf->unsent.offset > 0){
            word length = bf->unsent.offset;
            if(bf->unsent.offset > offset){
                length = offset;
            }
            bf->unsent.s->bytes -= length;
            bf->unsent.s->length += length;
            bf->unsent.s->alloc += length;
            bf->unsent.offset -= length;
            offset -= length;
            bf->unsent.total += length;
        }

        while(bf->unsent.s->length < offset){
            offset -= bf->unsent.s->length;
            bf->unsent.total += bf->unsent.s->length;
            bf->unsent.idx--;
            Buff_setUnsentStr(bf);
            if(bf->unsent.s == NULL){
                break;
            }
        }

        if(bf->unsent.s == NULL && offset > 0){
            bf->type.state |= ERROR;
            Error(bf->m, FUNCNAME, FILENAME, LINENUMBER,
                "Reached end of vec without using all of offset", NULL); 
            DebugStack_Pop();
            return bf->type.state;
        }

        if(offset > 0){
            bf->unsent.total += offset;
            word start = bf->unsent.s->length - offset;
            Buff_setUnsentIncr(bf, start);
        }
    }

    DebugStack_Pop();
    return bf->type.state;
}

static status Buff_posFrom(Buff *bf, i64 offset, i64 whence){
    DebugStack_Push(bf, bf->type.of);
    bf->type.state &= ~PROCESSING;
    Abstract *args[7];
    if((bf->type.state & (BUFF_FD|BUFF_SOCKET)) == 0){
        return Buff_vecPosFrom(bf, offset, whence);
    }

    i64 pos = lseek(bf->fd, offset, whence);
    if(pos < 0){
        Buff_Stat(bf);
        args[0] = (Abstract *)I64_Wrapped(bf->m, pos);
        args[1] = (Abstract *)I32_Wrapped(bf->m, offset);
        args[2] = (Abstract *)bf;
        args[3] = (Abstract *)Str_CstrRef(bf->m, Buff_WhenceChars(whence));
        args[6] = NULL;
        Error(bf->m, FUNCNAME, FILENAME, LINENUMBER,
            "Error seek failed with pos $ for offset $ on @ whence $", args);
        DebugStack_Pop();
        return ERROR;
    }else if(pos > 0){
        bf->type.state |= PROCESSING;
    }

    DebugStack_Pop();
    return bf->type.state;
}

i64 Buff_Bytes(Buff *bf, byte *bytes, i64 length){
    if(length > IO_SEND_MAX){
        Error(bf->m, FUNCNAME, FILENAME, LINENUMBER,
            "Error trying to send too many bytes at once", NULL);
        return 0;
    }
    if(bf->type.state & BUFF_UNBUFFERED){
        i64 offset = 0;
        i16 guard = 0;
        while(offset < length && (bf->type.state & ERROR) == 0){
            Guard_Incr(bf->m, &guard, BUFF_CYCLE_MAX, FUNCNAME, FILENAME, LINENUMBER);
            Buff_Unbuff(bf, bytes+offset, length-offset, &offset);
        }
        return length - (length - offset);
    }else if(Buff_AddBytes(bf, bytes, length) & SUCCESS){
        return length;
    }
    return 0;
}

status Buff_Stat(Buff *bf){
    if(bf->type.state & (BUFF_FD|BUFF_SOCKET)){
        if(fstat(bf->fd, &bf->st)){
            bf->type.state |= ERROR;
        }
    } else{
        bf->type.state |= ERROR;
    }
    return bf->type.state;
}

boolean Buff_Empty(Buff *bf){
    if(bf->type.state & (BUFF_FD|BUFF_SOCKET)){
        if((Buff_Stat(bf) & ERROR) == 0){
            return bf->st.st_size == 0;
        }else{
            return FALSE;
        }
    }else{
        return bf->v->total == 0;
    }
}

status Buff_SetFd(Buff *bf, i32 fd){
    bf->fd = fd;
    bf->type.state |= BUFF_FD;
    return SUCCESS;
}

status Buff_UnsetFd(Buff *bf){
    bf->fd = -1;
    bf->type.state &= ~BUFF_SOCKET;
    return SUCCESS;
}

status Buff_UnsetSocket(Buff *bf){
    bf->fd = -1;
    bf->type.state &= ~BUFF_SOCKET;
    return SUCCESS;
}

status Buff_SetSocket(Buff *bf, i32 fd){
    bf->fd = fd;
    bf->type.state |= BUFF_SOCKET;
    return SUCCESS;
}

status Buff_PosEnd(Buff *bf){
    return Buff_posFrom(bf, 0, SEEK_END);
}

status Buff_PosAbs(Buff *bf, i64 position){
    if(position < 0){
        return Buff_posFrom(bf, labs(position), SEEK_END);
    }else{
        return Buff_posFrom(bf, position, SEEK_SET);
    }
}

status Buff_Pos(Buff *bf, i64 position){
    return Buff_posFrom(bf, position, SEEK_CUR);
}

status Buff_AddBytes(Buff *bf, byte *bytes, i64 length){
    DebugStack_Push(bf, bf->type.of);
    status r = READY;
    if(bf->tail.idx == -1){
        bf->tail.s = Str_Make(bf->m, STR_DEFAULT);
        Span_Add(bf->v->p, (Abstract *)bf->tail.s);
        bf->tail.idx = bf->v->p->max_idx;
    }
    word remaining = bf->tail.s->alloc - bf->tail.s->length;
    i16 guard = 0;
    while(length > 0){
        Guard_Incr(bf->m, &guard, BUFF_CYCLE_MAX, FUNCNAME, FILENAME, LINENUMBER);
        if(length > remaining){
            Str_Add(bf->tail.s, bytes, remaining);
            bf->unsent.total += remaining;
            bf->v->total += remaining;
            bytes += remaining;
            length -= remaining;
            if(bf->tail.idx >= bf->v->p->max_idx){
                bf->tail.s = Str_Make(bf->m, STR_DEFAULT);
                Span_Add(bf->v->p, (Abstract *)bf->tail.s);
                bf->tail.idx = bf->v->p->max_idx;
            }
        }else{
            Str_Add(bf->tail.s, bytes, length);
            bf->unsent.total += length;
            bf->v->total += length;
            length -= length;
            r |= SUCCESS;
        }

        if(remaining == 0 && length > 0){
            Str *s = Str_Make(bf->m, STR_DEFAULT);
            Span_Add(bf->v->p, (Abstract *)s);
            bf->tail.idx = bf->v->p->max_idx;
            bf->tail.s = s;
            remaining = bf->tail.s->alloc;
        }
    }
    if(r == READY){
        r |= NOOP;
    }
    DebugStack_Pop();
    return r;
}

status Buff_RevGetStr(Buff *bf, Str *s){
    DebugStack_Push(bf, bf->type.of);
    word remaining = s->alloc - s->length;
    if(Buff_Pos(bf, -(remaining)) & ERROR){
        DebugStack_Pop();
        return ERROR;
    }
    status r = Buff_GetStr(bf, s);
    Buff_Pos(bf, -(remaining));
    DebugStack_Pop();
    return r;
}

status Buff_GetStr(Buff *bf, Str *s){
    DebugStack_Push(bf, bf->type.of);
    Abstract *args[5];
    bf->type.state &= ~(MORE|SUCCESS|ERROR|NOOP|PROCESSING|LAST);
    word remaining = s->alloc - s->length;

    if(bf->unsent.total > 0){
        if(bf->unsent.s == NULL){
            bf->unsent.idx = 0;
            Buff_setUnsentStr(bf);
        }
        i16 g = 0;

        while((bf->type.state & (MORE|SUCCESS|ERROR)) == 0 &&
                bf->unsent.total > 0 && remaining > 0){
            Guard_Incr(bf->m, &g, BUFF_CYCLE_MAX, FUNCNAME, FILENAME, LINENUMBER);
            if(bf->unsent.s->length > remaining){
                Str_Add(s, bf->unsent.s->bytes, remaining);
                Buff_setUnsentIncr(bf, remaining);
                bf->unsent.total -= remaining;
                remaining = 0;
            }else{
                Str_Add(s, bf->unsent.s->bytes, bf->unsent.s->length);
                bf->unsent.total -= bf->unsent.s->length;
                remaining -= bf->unsent.s->length;

                if(bf->unsent.idx == bf->v->p->max_idx){
                    bf->type.state |= PROCESSING;
                    bf->unsent.s = NULL;
                    bf->unsent.offset = 0;
                }else{
                    bf->unsent.idx++;
                    Buff_setUnsentStr(bf);
                    bf->type.state |= MORE;
                }
            }
        }
    }else{
        if(bf->type.state & (BUFF_FD|BUFF_SOCKET) && remaining > 0){
            Buff_ReadToStr(bf, s); 
        }
        bf->type.state |= (SUCCESS|END);
    }

    if(bf->unsent.total == 0){
        bf->type.state |= LAST;
    }

    DebugStack_Pop();
    return bf->type.state | (s->length == s->alloc ? SUCCESS : ZERO);
}

status Buff_GetToVec(Buff *bf, StrVec *v){
    DebugStack_Push(bf, bf->type.of);
    Str *shelf = Str_Make(bf->m, STR_DEFAULT);
    i16 g = 0;
    while((Buff_GetStr(bf, shelf) & END) == 0){
        Guard_Incr(bf->m, &g, BUFF_CYCLE_MAX, FUNCNAME, FILENAME, LINENUMBER);
        StrVec_Add(v, shelf);
        if((bf->type.state & LAST) == 0){
            shelf = Str_Make(bf->m, STR_DEFAULT);
        }
    }

    DebugStack_Pop();
    return bf->type.state;
}

status Buff_Add(Buff *bf, Str *s){
    Buff_AddBytes(bf, s->bytes, s->length);
    return bf->type.state;
}

status Buff_AddVec(Buff *bf, StrVec *v){
    DebugStack_Push(bf, bf->type.of);
    Iter it;
    Iter_Init(&it, v->p);
    while((Iter_Next(&it) & END) == 0){
        Str *s = Iter_Get(&it);
        Buff_AddBytes(bf, s->bytes, s->length);
    }
    DebugStack_Pop();
    return bf->type.state;
}

status Buff_Unbuff(Buff *bf, byte *bytes, i64 length, i64 *offset){
    return Buff_UnbuffFd(bf->m, bf->fd, bytes, length, bf->type.state, offset);
}

status Buff_UnbuffFd(MemCh *m, i32 fd, byte *bytes, i64 length, word flags, i64 *offset){
    if(length > IO_SEND_MAX || length < 0){
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "length of send unbuff is larger than IO_SEND_MAX or less than 0", NULL);
        flags |= ERROR;
        DebugStack_Pop();
        return flags;
    }

    ssize_t sent = 0;
    if(flags & BUFF_SOCKET){
        sent = send(fd, bytes, min(length, IO_BLOCK_SIZE), 0);
    }else if(flags & BUFF_FD){
        sent = write(fd, bytes, min(length, IO_BLOCK_SIZE));
    }else{
        Error(m, FUNCNAME, FILENAME, LINENUMBER, 
            "Buff Send requires the BUFF_SOCKET or BUFF_FD flag", NULL);
        flags |= ERROR;
        DebugStack_Pop();
        return flags;
    }

    *offset = sent;
    if(sent < 0){
        printf("Error sending str %d\n", fd);
        exit(1);

        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Error sending str", NULL);
        flags |= ERROR;
    }else if(sent == length){
        flags |= SUCCESS;
    }else{
        flags |= PROCESSING;
    }
    DebugStack_Pop();
    return flags;
}

status Buff_Send(Buff *bf){
    DebugStack_Push(bf, bf->type.of);
    status r = READY;
    if(bf->type.state & (ERROR)){
        DebugStack_Pop();
        return bf->type.state;
    }

    r |=  Buff_SendToFd(bf, bf->fd);    

    DebugStack_Pop();
    return r;
}

status Buff_SendToFd(Buff *bf, i32 fd){
    DebugStack_Push(bf, bf->type.of);
    bf->type.state &= ~(MORE|SUCCESS|NOOP);

    if(bf->unsent.total > 0){
        if(bf->unsent.s == NULL){
            bf->unsent.s = Str_Rec(bf->m, Span_Get(bf->v->p, 0));
        }
        Str *s = bf->unsent.s;
        ssize_t sent = 0;
        if(bf->type.state & BUFF_SOCKET){
            sent = send(fd, s->bytes, s->length, 0);
        }else if(bf->type.state & BUFF_FD){
            sent = write(fd, s->bytes, s->length);
        }else{
            Error(bf->m, FUNCNAME, FILENAME, LINENUMBER, 
                "Buff Send requires the BUFF_SOCKET or BUFF_FD flag", NULL);
            bf->type.state |= ERROR;
            DebugStack_Pop();
            return bf->type.state;
        }

        if(sent < 0){
            Error(bf->m, FUNCNAME, FILENAME, LINENUMBER,
                "Error sending str", NULL);
            bf->type.state |= ERROR;
        }else if(sent == s->length){
            if(bf->unsent.idx < bf->v->p->max_idx){
                bf->unsent.idx++;
                bf->unsent.s = Span_Get(bf->v->p, bf->unsent.idx);
                bf->type.state |= MORE;
            }else{
                bf->type.state |= PROCESSING;
                bf->unsent.idx = -1;
                bf->unsent.s = NULL;
                bf->unsent.offset = 0;
                bf->unsent.total -= sent;
                if(bf->type.state & BUFF_FLUSH){
                    Iter it;                    
                    Iter_Init(&it, bf->v->p);
                    while((Iter_Next(&it) & END) == 0){
                        if(it.idx == 0){
                            bf->tail.idx = 0;
                            bf->tail.s = (Str *)Iter_Get(&it);
                        }
                        Str_Wipe(Iter_Get(&it));
                    }
                    bf->unsent.idx = 0;
                    bf->unsent.s = bf->tail.s;
                    bf->unsent.total = 0;
                }
            }
        }else if(send > 0){
            Buff_setUnsentIncr(bf, sent);
            bf->type.state |= MORE;
        }else{
            bf->type.state |= NOOP;
        }
    }else{
        bf->type.state |= (SUCCESS|END);
    }
    DebugStack_Pop();
    return bf->type.state;
}

status Buff_AddSend(Buff *bf, Str *s){
    if((bf->type.state & BUFF_UNBUFFERED) == 0){
        Buff_Add(bf, s);
        if(bf->type.state & (BUFF_SOCKET|BUFF_FD)){
            while((Buff_Send(bf) & (SUCCESS|ERROR|END)) == 0){}
        }
    }else{
        i64 offset = 0;
        while(offset < s->alloc && (bf->type.state & ERROR) == 0){
            Buff_Unbuff(bf, s->bytes+offset, s->length-offset, &offset);
        }
    }
    return bf->type.state;
}

status Buff_SendAll(Buff *bf, StrVec *v){
    Iter it;
    Iter_Init(&it, v->p);
    while((Iter_Next(&it) & END) == 0){
        Str *s = (Str *)Iter_Get(&it);
        while((Buff_AddSend(bf, s) & (SUCCESS|ERROR|END)) == 0){}
        if(bf->type.state & ERROR){
            break;
        }
    }
    return bf->type.state;
}

status Buff_Read(Buff *bf){
    return Buff_ReadAmount(bf, IO_SEND_MAX);
}

status Buff_ReadToStr(Buff *bf, Str *s){
    DebugStack_Push(bf, bf->type.of);
    bf->type.state &= ~(SUCCESS|PROCESSING);
    i16 amount = s->alloc - s->length;
    byte *bytes = s->bytes+s->length;
    while(amount > 0){
        ssize_t recieved = 0;
        if(bf->type.state & BUFF_SOCKET){
            recieved = recv(bf->fd, bytes, amount, 0);
        }else if(bf->type.state & BUFF_FD){
            recieved = read(bf->fd, s->bytes, amount);
        }else{
            Error(bf->m, FUNCNAME, FILENAME, LINENUMBER, 
                "Buff Send requires the BUFF_SOCKET or BUFF_FD flag", NULL);
            bf->type.state |= ERROR;
            DebugStack_Pop();
            return bf->type.state;
        }

        if(recieved < 0){
            Abstract *args[] = {
                (Abstract *)I32_Wrapped(bf->m, bf->fd),
                (Abstract *)Str_CstrRef(bf->m, strerror(errno)),
                NULL
            };
            Error(bf->m, FUNCNAME, FILENAME, LINENUMBER,
                "Error reading from ^D.$^d.fd: $", args);
            bf->type.state |= ERROR;
            break;
        }else{
            s->length += recieved;
            amount -= recieved;
            bf->type.state |= PROCESSING;
            if(recieved == 0 || amount == 0){
                bf->type.state |= SUCCESS;
                break;
            }
        }
    }
    DebugStack_Pop();
    return bf->type.state;
}

status Buff_ReadAmount(Buff *bf, i64 amount){
    bf->type.state &= ~(SUCCESS|PROCESSING);

    if(amount > IO_SEND_MAX){
        Error(bf->m, FUNCNAME, FILENAME, LINENUMBER, 
            "Buff read amount larget than IO_SEND_MAX", NULL);
        bf->type.state |= ERROR;
        return bf->type.state;
    }

    while(amount > 0){
        ssize_t recieved = 0;
        Str *s = Str_Make(bf->m, min(amount, IO_BLOCK_SIZE));
        if(bf->type.state & BUFF_SOCKET){
            recieved = recv(bf->fd, s->bytes, s->alloc, 0);
        }else if(bf->type.state & BUFF_FD){
            recieved = read(bf->fd, s->bytes, s->alloc);
        }else{
            Error(bf->m, FUNCNAME, FILENAME, LINENUMBER, 
                "Buff Send requires the BUFF_SOCKET or BUFF_FD flag", NULL);
            bf->type.state |= ERROR;
            return bf->type.state;
        }

        if(recieved < 0){
            Abstract *args[] = {
                (Abstract *)I32_Wrapped(bf->m, bf->fd),
                (Abstract *)Str_CstrRef(bf->m, strerror(errno)),
                NULL
            };
            Error(bf->m, FUNCNAME, FILENAME, LINENUMBER,
                "Error reading from ^D.$^d.fd: $", args);
            bf->type.state |= ERROR;
            break;
        }else{
            s->length += recieved;
            Buff_Add(bf, s);
            amount -= recieved;
            bf->type.state |= PROCESSING;
            if(recieved == 0 || amount == 0){
                bf->type.state |= SUCCESS;
                break;
            }
        }
    }

    return bf->type.state;
}

Buff *Buff_From(MemCh *m, StrVec *v, word flags){
    Buff *bf = (Buff *)MemCh_AllocOf(m, sizeof(Buff), TYPE_BUFF);
    bf->type.of = TYPE_BUFF;
    bf->type.state = flags;
    bf->m = m;
    bf->fd = -1;
    bf->v = v;
    bf->tail.idx = -1;
    return bf;
}

Buff *Buff_Make(MemCh *m, word flags){
    return Buff_From(m, StrVec_Make(m), flags);
}
