#include <external.h>
#include <caneka.h>

status Send_Add(Buff *bf, Str *s){
    Buff_AddBytes(bf, s->bytes, s->length);
    return bf->type.state;
}

status Send_AddVec(Buff *bf, StrVec *v){
    Iter it;
    Iter_Init(&it, v->p);
    while((Iter_Next(&it) & END) == 0){
        Str *s = Iter_Get(&it);
        Buff_AddBytes(bf, s->bytes, s->length);
    }
    return bf->type.state;
}

status Send_Unbuff(Buff *bf, byte *bytes, word length){
    ssize_t sent = 0;
    if(bf->type.state & BUFF_SOCKET){
        sent = send(bf->fd, bytes, length, 0);
    }else if(bf->type.state & BUFF_FD){
        sent = write(bf->fd, bytes, length);
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
    }else if(sent == length){
        bf->type.state |= SUCCESS;
    }else{
        Error(bf->m, FUNCNAME, FILENAME, LINENUMBER,
            "Error sending did not send all bytes", NULL);
        bf->type.state |= ERROR;
    }
    return bf->type.state;
}

status Send_Send(Buff *bf){
    if((bf->type.state & BUFF_OUT) == 0){
        Error(bf->m, FUNCNAME, FILENAME, LINENUMBER, 
            "Buff Send requires the BUFF_OUT flag", NULL);
        bf->type.state |= ERROR;
        return bf->type.state;
    }

    if(bf->type.state & BUFF_STRVEC){
        return NOOP;
    }

    if(bf->type.state & (ERROR)){
        return bf->type.state;
    }

    bf->type.state &= ~(MORE|SUCCESS|NOOP);

    if(bf->unsent.total > 0){
        Str *s = bf->unsent.s;
        ssize_t sent = 0;
        if(bf->type.state & BUFF_SOCKET){
            sent = send(bf->fd, s->bytes, s->length, 0);
        }else if(bf->type.state & BUFF_FD){
            sent = write(bf->fd, s->bytes, s->length);
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
                bf->type.state |= SUCCESS;
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
    }
    return bf->type.state;
}

status Send_AddSend(Buff *bf, Str *s){
    Send_Add(bf, s);
    return Send_Send(bf);
}

status Send_SendAll(Buff *bf, StrVec *v){
    Iter it;
    Iter_Init(&it, v->p);
    while((Iter_Next(&it) & END) == 0){
        Str *s = (Str *)Iter_Get(&it);
        while((Send_AddSend(bf, s) & (SUCCESS|ERROR|NOOP)) == 0){}
        if(bf->type.state & ERROR){
            break;
        }
    }
    return bf->type.state;
}
