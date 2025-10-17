#include <external.h>
#include <caneka.h>

status TcpTask_ReadToRbl(Step *st, Task *tsk){
    struct pollfd *pfd = TcpTask_GetPollFd(tsk);
    ProtoCtx *proto = (ProtoCtx *)as(tsk->data, TYPE_PROTO_CTX);
    Roebling *rbl = (Roebling *)as(st->arg, TYPE_ROEBLING);

    byte buff[SERV_READ_SIZE];
    buff[0] = '\0';
    ssize_t l = recv(pfd->fd, buff, SERV_READ_SIZE, 0);
    status r = NOOP;
    if(l > 0){
        Str *s = Str_From(tsk->m, buff, l);
        Cursor_Add(rbl->curs, s);
        r |= Roebling_Run(rbl);
    }

    if(tsk->type.state & DEBUG){
        Abstract *args[] = {
            (Abstract *)rbl->curs->v,
            NULL,
        };
        Out("^0.ReadToRbl -> ^g.&^0\n", args);
    }
    
    st->type.state |= (rbl->type.state & (SUCCESS|ERROR));
    if(st->type.state & SUCCESS && (tsk->type.state & DEBUG)){
        Abstract *args[] = {
            (Abstract *)tsk->data,
            NULL,
        };
        Out("^0.Parsed Tcp Initial Request -> ^g.&/^p@^0\n", args);
    }

    return st->type.state;
}

status TcpTask_WriteFromOut(Step *st, Task *tsk){
    struct pollfd *pfd = TcpTask_GetPollFd(tsk);
    ProtoCtx *proto = (ProtoCtx *)as(tsk->data, TYPE_PROTO_CTX);

    ssize_t l = 0;
    i32 total = 0;
    Str s;
    memset(&s, 0, sizeof(Str));
    Str_Init(&s, NULL, 0, 0);
    while(total < SERV_READ_SIZE && (proto->out->type.state & END) == 0){
        if(Cursor_SetStrBytes(proto->out, &s, SERV_READ_SIZE-total) & NOOP){
            break;
        }
        if(s.length > 0){
            l = write(pfd->fd, s.bytes, s.length); 
            if(l > 0){
                Cursor_Incr(proto->out, (i32)l);
                total += l;
            }else{
                if(l < 0){
                    tsk->type.state |= ERROR;
                } 
                break;
            }
        }else{
            break;
        }
    }

    if(tsk->type.state & DEBUG){
        Abstract *args[] = {
            (Abstract *)StreamTask_Make(OutStream->m, NULL, (Abstract *)proto->out, ToS_FlagLabels),
            (Abstract *)proto->out->v,
            NULL,
        };
        Out("Sent($): ^g.&^0\n", args);
    }

    if(proto->out->type.state & END){
        st->type.state |= SUCCESS;
    }

    st->type.state |= (proto->out->type.state & ERROR);
    
    return st->type.state;
}

status TcpTask_ExpectRecv(Step *st, Task *tsk){
    struct pollfd *pfd = TcpTask_GetPollFd(tsk);
    pfd->events = POLL_IN;
    return TASK_UPDATE_CRIT;
}

status TcpTask_ExpectSend(Step *st, Task *tsk){
    struct pollfd *pfd = TcpTask_GetPollFd(tsk);
    pfd->events = POLL_OUT;
    return TASK_UPDATE_CRIT;
}
