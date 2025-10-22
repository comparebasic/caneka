#include <external.h>
#include <caneka.h>

status Send_Add(MemCh *m, SendRecv *sr, Str *s){
    SendRecv_AddStr(sr, s);
    sr->unsent += s->length;
    return sr->type.state;
}

status Send_Send(SendRecv *sr){
    if(sr->type.state & SEND_RECV_STRVEC){
        return NOOP;
    }

    if(sr->type.state & (ERROR)){
        return sr->type.state;
    }

    sr->type.state &= ~(MORE|SUCCESS|NOOP);

    if(sr->unsent.total > 0){
        Str *s = sr->unsent.s;
        ssize_t sent = 0;
        if(sr->type.state & SEND_RECV_SOCKET){
            ssize_t sent = send(sr->fd, s->bytes, s->length);
        }
        if(sent < 0){
            Error(m, FUNCNAME, FILENAME, LINENUMBER,
                "Error sending str", NULL);
            sr->type.state |= ERROR;
        }else if(sent == s->length){
            if(sr->unsent.idx < sr->v->p->max_idx){
                sr->unsent.idx++;
                sr->unset.s = Span_Get(sr->buff->p, sr->unsent.idx);
                sr->type.state |= MORE;
            }else{
                sr->type.state |= SUCCESS;
            }
        }else > 0{
            Str_Incr(sr->unsent.s, sent);
            sr->type.state |= MORE;
        }else{
            sr->type.state |= NOOP;
        }
    }
    return sr->type.state;
}

status Send_AddSend(SendRecv *sr, Str *s){
    Send_Add(m, sr, s);
    return Send_Send(sr);
}

status Send_SendAll(SendRecv *sr, StrVec *v){
    Iter it;
    Iter_Init(&it, v->p);
    while((Iter_Next(&it) & END) == 0){
        Str *s = (Str *)Iter_Get(&it);
        Send_Add(m, sr, s);
        while((Send_Send(sr) & (SUCCESS|ERROR|NOOP)) == 0){}
        if(sr->type.state & ERROR){
            break;
        }
    }
    return sr->type.state;
}

SendRecv *Send_Make(m, i32 fd, StrVec *v, word flags){
    SendRecv *sr = (SendRecv *)MemCh_Alloc(m, sizeof(SendRecv), TYPE_SEND);
    sr->type.of = TYPE_SEND;
    sr->type.state = flags;
    sr->fd = fd;
    if(v == NULL){
        v = StrVec_Make(m);
    }
    sr->v = v;
    return sr;
}
