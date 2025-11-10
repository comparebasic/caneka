#include <external.h>
#include <caneka.h>

static void Buff_addTail(Buff *bf){
    Str *s = Str_Make(bf->m, STR_DEFAULT);
    Span_Add(bf->v->p, s);
    bf->tail.idx = bf->v->p->max_idx;
    bf->tail.s = s;
}

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
    void *args[5];
    if(whence == SEEK_END){
        if(offset < 0){
            args[0] = bf;
            args[1] = I32_Wrapped(bf->m, offset);
            args[2] = NULL;
            Error(bf->m, FUNCNAME, FILENAME, LINENUMBER,
                "Offset cannot be zero if seeking from the end $: $", args);
            bf->type.state |= ERROR;
            return bf->type.state;
        }
        bf->unsent.idx = bf->v->p->max_idx;
        if(offset == 0){
            bf->type.state |= END;
            bf->unsent.s = NULL;
            bf->unsent.total = 0;
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
                args[0] = bf;
                args[1] = I32_Wrapped(bf->m, offset);
                args[2] = I32_Wrapped(bf->m, bf->v->total);
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
            return bf->type.state;
        }

        if(offset > 0){
            bf->unsent.total += offset;
            word start = bf->unsent.s->length - offset;
            Buff_setUnsentIncr(bf, start);
        }
    }

    return bf->type.state;
}

static status Buff_posFrom(Buff *bf, i64 offset, i64 whence){
    bf->type.state &= ~PROCESSING;
    void *args[7];
    if((bf->type.state & (BUFF_FD|BUFF_SOCKET)) == 0){
        return Buff_vecPosFrom(bf, offset, whence);
    }

    i64 pos = lseek(bf->fd, offset, whence);
    if(pos < 0){
        Buff_Stat(bf);
        args[0] = I64_Wrapped(bf->m, pos);
        args[1] = I32_Wrapped(bf->m, offset);
        args[2] = bf;
        args[3] = Str_CstrRef(bf->m, Buff_WhenceChars(whence));
        args[6] = NULL;
        Error(bf->m, FUNCNAME, FILENAME, LINENUMBER,
            "Error seek failed with pos $ for offset $ on @ whence $", args);
        return ERROR;
    }else if(pos > 0){
        bf->type.state |= PROCESSING;
    }

    return bf->type.state;
}

static status Buff_bytesToFd(Buff *bf, i32 fd, byte *bytes, i64 length, i64 *offset){
    MemCh *m = bf->m;
    bf->type.state &= ~(SUCCESS|MORE|PROCESSING);
    if(length > IO_SEND_MAX || length < 0){
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "length of send unbuff is larger than IO_SEND_MAX or less than 0", NULL);
        bf->type.state |= ERROR;
        return bf->type.state;
    }

    ssize_t sent = 0;
    if(bf->type.state & BUFF_SOCKET){
        sent = send(fd, bytes, min(length, IO_BLOCK_SIZE), 0);
    }else{
        sent = write(fd, bytes, min(length, IO_BLOCK_SIZE));
    }

    *offset = *offset+sent;
    if(sent < 0){
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Error sending str", NULL);
        bf->type.state |= ERROR;
    }else if(sent == length){
        bf->type.state |= SUCCESS;
    }else if(sent == 0 && length > 0){
        bf->type.state |= MORE;
    }else{
        bf->type.state |= PROCESSING;
    }
    return bf->type.state;
}

static status Buff_sendToFd(Buff *bf, i32 fd){
    bf->type.state &= ~(MORE|SUCCESS|NOOP|PROCESSING);
    void *args[5];
    if(bf->unsent.total > 0){
        if(bf->unsent.s == NULL){
            bf->unsent.s = Str_Rec(bf->m, Span_Get(bf->v->p, max(0, bf->unsent.idx)));
            Str_Incr(bf->unsent.s, bf->unsent.offset);
        }
        Str *s = bf->unsent.s;

        byte *bytes = s->bytes;
        i64 length = s->length;
        i64 offset = 0;
        i16 guard = 0;
        while((bf->type.state & (SUCCESS|ERROR|MORE)) == 0){
            Guard_Incr(bf->m, &guard, BUFF_CYCLE_MAX, FUNCNAME, FILENAME, LINENUMBER);
            Buff_bytesToFd(bf, fd, bytes+offset, length-offset, &offset);
        }

        bf->type.state &= ~(SUCCESS|MORE);

        if(bf->type.state & ERROR){
            return bf->type.state;
        }

        if(offset == s->length){
            Buff_setUnsentIncr(bf, offset);
            if(bf->unsent.s->length == 0){
                if(bf->unsent.idx < bf->v->p->max_idx){
                    bf->unsent.idx++;
                    bf->unsent.s = Span_Get(bf->v->p, bf->unsent.idx);
                    bf->type.state |= MORE;
                    bf->unsent.total -= offset;
                }else{
                    bf->type.state |= PROCESSING;
                    bf->unsent.s = NULL;
                    bf->unsent.total -= offset;
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
            }
        }else if(offset > 0){
            Buff_setUnsentIncr(bf, offset);
            bf->type.state |= MORE;
        }else{
            bf->type.state |= NOOP;
        }
    }else{
        bf->type.state |= (SUCCESS|END);
    }
    return bf->type.state;
}

status Buff_SetFd(Buff *bf, i32 fd){
    bf->fd = fd;
    bf->type.state |= BUFF_FD;
    return SUCCESS;
}

status Buff_SetSocket(Buff *bf, i32 fd){
    bf->fd = fd;
    bf->type.state |= BUFF_SOCKET;
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

boolean Buff_IsEmpty(Buff *bf){
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

status Buff_Stat(Buff *bf){
    memset(&bf->st, 0, sizeof(struct stat));
    if(bf->type.state & (BUFF_FD|BUFF_SOCKET)){
        if(fstat(bf->fd, &bf->st)){
            bf->type.state |= ERROR;
        }
    } else{
        bf->st.st_size = bf->v->total;
    }
    return bf->type.state;
}

status Buff_GetStr(Buff *bf, Str *s){
    void *args[5];
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
        }else{
            bf->type.state |= (SUCCESS|END);
        }
    }

    if(bf->unsent.total == 0){
        bf->type.state |= LAST;
    }

    return bf->type.state | (s->length == s->alloc ? SUCCESS : ZERO);
}

status Buff_RevGetStr(Buff *bf, Str *s){
    word remaining = s->alloc - s->length;
    if(Buff_Pos(bf, -(remaining)) & ERROR){
        return ERROR;
    }
    status r = Buff_GetStr(bf, s);
    Buff_Pos(bf, -(remaining));
    return r;
}

status Buff_GetToVec(Buff *bf, StrVec *v){
    Str *shelf = Str_Make(bf->m, STR_DEFAULT);
    i16 g = 0;
    while((Buff_GetStr(bf, shelf) & END) == 0){
        Guard_Incr(bf->m, &g, BUFF_CYCLE_MAX, FUNCNAME, FILENAME, LINENUMBER);
        StrVec_Add(v, shelf);
        if((bf->type.state & LAST) == 0){
            shelf = Str_Make(bf->m, STR_DEFAULT);
        }
    }

    return bf->type.state;
}

status Buff_Add(Buff *bf, Str *s){
    Buff_AddBytes(bf, s->bytes, s->length);
    return bf->type.state;
}

status Buff_AddVec(Buff *bf, StrVec *v){
    Iter it;
    Iter_Init(&it, v->p);
    while((Iter_Next(&it) & END) == 0){
        Str *s = Iter_Get(&it);
        Buff_AddBytes(bf, s->bytes, s->length);
    }
    return bf->type.state;
}

status Buff_AddBytes(Buff *bf, byte *bytes, i64 length){
    status r = READY;
    if(length <= 0){
        return NOOP;
    }

    if(bf->type.state & ERROR){
        void *args[2];
        args[0] = bf;
        args[1] = NULL;
        Error(bf->m, FUNCNAME, FILENAME, LINENUMBER,
            "Error error flags set on buffer", args);
        return bf->type.state;
    }

    bf->type.state &= ~SUCCESS|NOOP;

    if(length > IO_SEND_MAX){
        Error(bf->m, FUNCNAME, FILENAME, LINENUMBER,
            "Error trying to send too many bytes at once", NULL);
        return 0;
    }

    bf->type.state &= ~END;
    if(bf->type.state & BUFF_UNBUFFERED){
        if((bf->type.state & (BUFF_SOCKET|BUFF_FD)) == 0){
            Error(bf->m, FUNCNAME, FILENAME, LINENUMBER,
                "Error need a socket or fd for an unuffered buff", NULL);
            bf->type.state |= ERROR;
            return bf->type.state;
        }

        Str s = {
            .type = {TYPE_STR, ZERO},
            .alloc = length,
            .length = length,
            .bytes = bytes,
        };

        i16 guard = 0;
        i64 offset = 0;
        while((bf->type.state & (SUCCESS|ERROR|MORE)) == 0){
            Guard_Incr(bf->m, &guard, BUFF_CYCLE_MAX, FUNCNAME, FILENAME, LINENUMBER);
            Buff_bytesToFd(bf,
                bf->fd, bytes+offset, length-offset, &offset);
        }

        return bf->type.state;
    }

    if(bf->tail.idx == -1){
        Buff_addTail(bf);
    }

    i16 guard = 0;
    while(length > 0){
        Guard_Incr(bf->m, &guard, BUFF_CYCLE_MAX, FUNCNAME, FILENAME, LINENUMBER);
        word remaining = bf->tail.s->alloc - bf->tail.s->length;
        if(remaining == 0){
            Buff_addTail(bf);
        }else if(length <= remaining){
            Str_Add(bf->tail.s, bytes, length);
            bf->unsent.total += length;
            bf->v->total += length;
            length -= length;
            r |= SUCCESS;
        }else{
            Str_Add(bf->tail.s, bytes, remaining);
            bf->unsent.total += remaining;
            bf->v->total += remaining;
            bytes += remaining;
            length -= remaining;
            Buff_addTail(bf);
        }

    }
    if(r == READY){
        r |= NOOP;
    }

    return r;
}

status Buff_Flush(Buff *bf){
    if(bf->type.state & DEBUG){
        void *args[2];
        args[0] = bf;
        args[1] = NULL;
        Out("^pFlushing @\n", args);
    }
    if(bf->type.state & (BUFF_SOCKET|BUFF_FD)){
        while((Buff_sendToFd(bf, bf->fd) & (SUCCESS|ERROR|NOOP|END)) == 0){
            if(bf->type.state & DEBUG){
                void *args[2];
                args[0] = bf;
                args[1] = NULL;
                Out("^p   cycle - Flushing @\n", args);
            }
        }
    }

    if(bf->type.state & DEBUG){
        void *args[2];
        args[0] = bf;
        args[1] = NULL;
        Out("^p    flushed - Flushing @\n", args);
    }
    return bf->type.state;
}

status Buff_Read(Buff *bf){
    return Buff_ReadAmount(bf, IO_SEND_MAX);
}

status Buff_ReadAmount(Buff *bf, i64 amount){
    bf->type.state &= ~(SUCCESS|PROCESSING|MORE|NOOP);

    if(amount > IO_SEND_MAX){
        Error(bf->m, FUNCNAME, FILENAME, LINENUMBER, 
            "Buff read amount larget than IO_SEND_MAX", NULL);
        bf->type.state |= ERROR;
        return bf->type.state;
    }

    i64 orig = amount;
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
            if(bf->type.state & BUFF_FD){
                void *args[] = {
                    I32_Wrapped(bf->m, bf->fd),
                    Str_CstrRef(bf->m, strerror(errno)),
                    NULL
                };
                Error(bf->m, FUNCNAME, FILENAME, LINENUMBER,
                    "Error reading from ^D.$^d.fd: $", args);
                bf->type.state |= ERROR;
            }
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
    if(amount == orig){
        bf->type.state |= NOOP;
    }

    if(amount > 0){
        bf->type.state |= MORE;
    }

    return bf->type.state;
}

status Buff_ReadToStr(Buff *bf, Str *s){
    bf->type.state &= ~(SUCCESS|PROCESSING);
    i16 amount = s->alloc - s->length;
    byte *bytes = s->bytes+s->length;
    while(amount > 0){
        ssize_t recieved = 0;
        if(bf->type.state & BUFF_SOCKET){
            recieved = recv(bf->fd, bytes, amount, 0);
        }else if(bf->type.state & BUFF_FD){
            recieved = read(bf->fd, bytes, amount);
        }else{
            Error(bf->m, FUNCNAME, FILENAME, LINENUMBER, 
                "Buff Send requires the BUFF_SOCKET or BUFF_FD flag", NULL);
            bf->type.state |= ERROR;
            return bf->type.state;
        }

        if(recieved < 0){
            void *args[] = {
                I32_Wrapped(bf->m, bf->fd),
                Str_CstrRef(bf->m, strerror(errno)),
                NULL
            };
            Error(bf->m, FUNCNAME, FILENAME, LINENUMBER,
                "Error reading from ^D.$^d.fd: $", args);
            bf->type.state |= ERROR;
            break;
        }else{
            s->length += recieved;
            amount -= recieved;
            bytes += recieved;
            bf->type.state |= PROCESSING;
            if(recieved == 0){
                bf->type.state |= END;
            }
            if(amount == 0){
                bf->type.state |= SUCCESS;
            }
            break;
        }
    }
    return bf->type.state;
}

status Buff_Pipe(Buff *to, Buff *from){
    Str *s = Str_Make(to->m, IO_BLOCK_SIZE);
    status r = READY;
    while((from->type.state & (END|ERROR)) == 0){
        Buff_GetStr(from, s);
        r |= Buff_AddBytes(to, s->bytes, s->length);
        s->length = 0;
    }
    return r;
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

status Buff_PosEnd(Buff *bf){
    return Buff_posFrom(bf, 0, SEEK_END);
}

Buff *Buff_From(MemCh *m, StrVec *v){
    Buff *bf = (Buff *)MemCh_AllocOf(m, sizeof(Buff), TYPE_BUFF);
    bf->type.of = TYPE_BUFF;
    bf->m = m;
    bf->fd = -1;
    bf->v = v;
    bf->tail.idx = -1;
    bf->unsent.idx = 0;
    bf->unsent.s = Span_Get(v->p, bf->unsent.idx);
    bf->unsent.total = v->total;
    return bf;
}

Buff *Buff_Make(MemCh *m, word flags){
    Buff *bf = Buff_From(m, StrVec_Make(m));
    bf->type.state = flags;
    return bf;
}
