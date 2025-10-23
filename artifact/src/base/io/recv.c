#include <external.h>
#include <caneka.h>

status Recv_Get(Buff *bf, Str *s){
    Abstract *args[5];
    bf->type.state &= ~(MORE|SUCCESS|ERROR|NOOP|PROCESSING|LAST);
    if(bf->unsent.total > 0){
        if(bf->unsent.s == NULL){
            bf->unsent.s = Str_Rec(bf->m, Span_Get(bf->v->p, 0));
        }
        i16 g = 0;
        word remaining = s->alloc - s->length;
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
        bf->type.state |= (SUCCESS|END);
    }

    if(bf->unsent.total == 0){
        bf->type.state |= LAST;
    }

    if(bf->type.state & DEBUG){
        args[0] = (Abstract *)s;
        args[1] = (Abstract *)bf;
        args[2] = NULL;
        Out("^p.Recv_Get copied:@ from:@^0\n", args);
    }

    return bf->type.state;
}

status Recv_GetToVec(Buff *bf, StrVec *v){
    Str *shelf = Str_Make(bf->m, STR_DEFAULT);
    i16 g = 0;
    while((Recv_Get(bf, shelf) & END) == 0){
        Guard_Incr(bf->m, &g, BUFF_CYCLE_MAX, FUNCNAME, FILENAME, LINENUMBER);
        StrVec_Add(v, shelf);
        if((bf->type.state & LAST) == 0){
            shelf = Str_Make(bf->m, STR_DEFAULT);
        }
    }

    return bf->type.state;
}

status Recv_GetToFd(Buff *bf, i32 fd, i64 length, i64 *offset){
    bf->type.state &= ~(MORE|SUCCESS|ERROR|NOOP|PROCESSING);
    if(bf->unsent.total > 0){
        if(bf->unsent.s == NULL){
            bf->unsent.s = Span_Get(bf->v->p, 0);
        }
        while((bf->type.state & (MORE|SUCCESS|ERROR)) == 0 &&
                bf->unsent.s != NULL && bf->unsent.s->length > 0){
            word sendlen = length;
            if(bf->unsent.s->length < length){
                sendlen = bf->unsent.s->length;
            }

            ssize_t sent = 0;
            if(bf->type.state & BUFF_SOCKET){
                sent = send(bf->fd, bf->unsent.s->bytes, sendlen, 0);
            }else if(bf->type.state & BUFF_FD){
                sent = write(bf->fd, bf->unsent.s->bytes, sendlen);
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
