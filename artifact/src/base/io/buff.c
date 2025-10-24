#include <external.h>
#include <caneka.h>

static status Buff_posFrom(Buff *bf, i32 offset, i32 whence){
    Abstract *args[4];
    if(bf->type.state & (BUFF_FD|BUFF_SOCKET)){
        bf->type.state &= ~(BUFF_FD|BUFF_SOCKET);
    }else{
        args[0] = (Abstract *)bf;
        args[1] = NULL;
        Error(bf->m, FUNCNAME, FILENAME, LINENUMBER,
            "Error cannot seek on a Buff that  does not have a BUFF_SOCKET or "
            "BUFF_FD flag @", args);
        return ERROR;
    }

    i32 pos = lseek(bf->fd, whence, offset);
    if(pos < 0){
        args[0] = (Abstract *)bf;
        args[1] = (Abstract *)I32_Wrapped(bf->m, offset);
        args[3] = NULL;
        Error(bf->m, FUNCNAME, FILENAME, LINENUMBER,
            "Error seek failed on a Buff for offset $", args);
        return ERROR;
    }
    return SUCCESS;
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

status Buff_PosAbs(Buff *bf, i32 position){
    if(position < 0){
        return Buff_posFrom(bf, abs(position), SEEK_END);
    }else{
        return Buff_posFrom(bf, position, SEEK_SET);
    }
}

status Buff_Pos(Buff *bf, i32 position){
    return Buff_posFrom(bf, position, SEEK_CUR);
}

status Buff_AddBytes(Buff *bf, byte *bytes, word length){
    status r = READY;
    if(bf->tail.idx == -1){
        bf->tail.s = Str_Make(bf->m, STR_DEFAULT);
        Span_Add(bf->v->p, (Abstract *)bf->tail.s);
        bf->tail.idx = bf->v->p->max_idx;
    }
    word remaining = bf->tail.s->alloc - bf->tail.s->length;
    while(length > 0){
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
    }
    if(r == READY){
        r |= NOOP;
    }
    return r;
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

status Buff_GetStr(Buff *bf, Str *s){
    Abstract *args[5];
    bf->type.state &= ~(MORE|SUCCESS|ERROR|NOOP|PROCESSING|LAST);
    word remaining = s->alloc - s->length;
    if(bf->unsent.total > 0){
        if(bf->unsent.s == NULL){
            bf->unsent.s = Str_Rec(bf->m, Span_Get(bf->v->p, 0));
        }
        i16 g = 0;
        while((bf->type.state & (MORE|SUCCESS|ERROR)) == 0 &&
                bf->unsent.total > 0 && remaining > 0){
            Guard_Incr(bf->m, &g, BUFF_CYCLE_MAX, FUNCNAME, FILENAME, LINENUMBER);
            if(bf->unsent.s->length > remaining){
                Str_Add(s, bf->unsent.s->bytes, remaining);
                Str_Incr(bf->unsent.s, remaining);
                bf->unsent.total -= remaining;
                remaining = 0;
            }else{
                Str_Add(s, bf->unsent.s->bytes, bf->unsent.s->length);
                bf->unsent.total -= bf->unsent.s->length;
                if(bf->unsent.idx == bf->v->p->max_idx){
                    bf->type.state |= PROCESSING;
                    bf->unsent.s = NULL;
                }else{
                    bf->unsent.idx++;
                    bf->unsent.s = Str_Rec(bf->m, Span_Get(bf->v->p, bf->unsent.idx));
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

    if(bf->type.state & DEBUG){
        args[0] = (Abstract *)s;
        args[1] = (Abstract *)bf;
        args[2] = NULL;
        Out("^p.Buff_GetStr copied:@ from:@^0\n", args);
    }

    return bf->type.state;
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

status Buff_Unbuff(Buff *bf, byte *bytes, i64 length, i64 *offset){
    return Buff_UnbuffFd(bf->m, bf->fd, bytes, length, bf->type.state, offset);
}

status Buff_UnbuffFd(MemCh *m, i32 fd, byte *bytes, i64 length, word flags, i64 *offset){
    if(length > IO_SEND_MAX || length < 0){
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "length of send unbuff is larger than IO_SEND_MAX or less than 0", NULL);
        flags |= ERROR;
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
        return flags;
    }

    *offset = sent;
    if(sent < 0){
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Error sending str", NULL);
        flags |= ERROR;
    }else if(sent == length){
        flags |= SUCCESS;
    }else{
        flags |= PROCESSING;
    }
    return flags;
}

status Buff_Send(Buff *bf){
    if(bf->type.state & (ERROR)){
        return bf->type.state;
    }

    return Buff_SendToFd(bf, bf->fd);    
}

status Buff_SendToFd(Buff *bf, i32 fd){
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
                }
            }
        }else if(send > 0){
            Str_Incr(bf->unsent.s, sent);
            bf->type.state |= MORE;
        }else{
            bf->type.state |= NOOP;
        }
    }else{
        bf->type.state |= (SUCCESS|END);
    }
    return bf->type.state;
}

status Buff_AddSend(Buff *bf, Str *s){
    Buff_Add(bf, s);
    while((Buff_Send(bf) & (SUCCESS|ERROR|END)) == 0){}
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

Buff *Buff_Make(MemCh *m, word flags){
    Buff *bf = (Buff *)MemCh_AllocOf(m, sizeof(Buff), TYPE_BUFF);
    bf->type.of = TYPE_BUFF;
    bf->type.state = flags;
    bf->m = m;
    bf->fd = -1;
    bf->v = StrVec_Make(m);
    bf->tail.idx = -1;
    return bf;
}
