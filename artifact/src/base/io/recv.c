#include <external.h>
#include <caneka.h>

status Recv_Get(Buff *bf, Str *s){
    bf->type.state &= ~(MORE|SUCCESS|ERROR|NOOP|PROCESSING);
    if(bf->unsent.total > 0){
        if(bf->unsent.s == NULL){
            bf->unsent.s = Span_Get(bf->v.p, 0);
        }
        while((bf->type.state & (MORE|SUCCESS|ERROR)) == 0 &&
                bf->unsent.s != NULL && bf->unsent.length > 0){
            if(bf->unsent.s->length > s->length){
                Str_Add(s, bf->unsent.s->bytes, s->length);
                Str_Incr(bf->unsent.s, s->length);
                bf->type.state |= MORE;
                bf->unsent.total -= s->length;
            }else{
                Str_Add(s, bf->unsent.s->bytes, bf->unsent.s->length);
                bf->unsent.total -= bf->unsent.s->length;
                if(bf->unset.idx == bf->v.p->max_idx){
                    bf->type.state |= PROCESSING;
                    bf->unsent.s = NULL;
                }else{
                    bf->unsent.idx++;
                    bf->unset.s = Span_Get(bf->v.p, bf->insent.idx);
                }
            }
        }
    }else{
        bf->type.state |= NOOP;
    }

    if(bf->unsent.total == 0){
        bf->type.state |= SUCCESS;
    }
    return bf->type.state;
}

status Recv_GetToVec(Buff *bf, StrVec *v){
    word offset = 0;
    while((Recv_Get(bf, Str_Make(m, STR_DEFAULT)) & (MORE|SUCCESS|NOOP|ERROR)) == MORE){}
    return bf->type.state;
}

status Recv_GetToFd(Buff *bf, i32 fd, i64 length, i64 *offset){
    bf->type.state &= ~(MORE|SUCCESS|ERROR|NOOP|PROCESSING);
    if(bf->unsent.total > 0){
        if(bf->unsent.s == NULL){
            bf->unsent.s = Span_Get(bf->v.p, 0);
        }
        while((bf->type.state & (MORE|SUCCESS|ERROR)) == 0 &&
                bf->unsent.s != NULL && bf->unsent.length > 0){
            word sendlen = length;
            if(bf->unsent.s->length < length){
                sendlen = bf->unsent.s->length;
            }

            ssize_t sent = 0;
            if(bf->type.state & SEND_RECV_SOCKET){
                sent = send(bf->fd, bf->unsent.s->bytes, sendlen, 0);
            }else if(bf->type.state & SEND_RECV_FD){
                sent = write(bf->fd, bf->unsent.s->bytes, sendlen);
            }else{
                Error(bf->m, FUNCNAME, FILENAME, LINENUMBER, 
                    "Buff Send requires the BUFF_SOCKET or BUFF_FD flag", NULL);
                bf->type.state |= ERROR;
                return bf->type.state;
            }

            if(sent < 0){
                Error(m, FUNCNAME, FILENAME, LINENUMBER,
                    "Error sending str", NULL);
                bf->type.state |= ERROR;
                break;
            }

            if(sent < sendlen){
                bf->type.state |= MORE;
            }else{
                bf->type.state |= PROCESSING;
            }
            Str_Incr(bf->unsent.s, sent);
            bf->unsent.total -= sent;
            *offset = length - sent;
        }
    }else{
        bf->type.state |= NOOP;
    }

    if(bf->unsent.total == 0){
        bf->type.state |= SUCCESS;
    }
    return bf->type.state;
}
