#include <external.h>
#include <caneka.h>

status TcpTask_ReadToRbl(Step *st, Task *tsk){
    DebugStack_Push(st, st->type.of);
    struct pollfd *pfd = TcpTask_GetPollFd(tsk);
    ProtoCtx *proto = (ProtoCtx *)as(tsk->data, TYPE_PROTO_CTX);
    Roebling *rbl = (Roebling *)as(st->arg, TYPE_ROEBLING);

    Buff_SetSocket(proto->in, pfd->fd);
    if((Buff_ReadAmount(proto->in, SERV_READ_SIZE) & NOOP) == 0){
        Roebling_Run(rbl);
    }
    
    st->type.state |= (rbl->type.state & (SUCCESS|ERROR));
    if(st->type.state & SUCCESS && (tsk->type.state & DEBUG)){
        void *args[] = {
            tsk->data,
            NULL,
        };
        Out("^0.Parsed Tcp Initial Request -> ^c.&/@^0\n", args);
    }

    DebugStack_Pop();
    return st->type.state;
}

status TcpTask_WriteStep(Step *st, Task *tsk){
    DebugStack_Push(st, st->type.of);
    status r = READY;
    struct pollfd *pfd = TcpTask_GetPollFd(tsk);
    ProtoCtx *proto = (ProtoCtx *)as(tsk->data, TYPE_PROTO_CTX);

    Buff_SetFd(proto->out, pfd->fd);

    Iter it;
    Iter_Init(&it, proto->outSpan);
    while((Iter_Next(&it) & END) == 0){
        Buff *bf = Iter_Get(&it);


        Buff_Pipe(proto->out, bf);
        if(bf->type.state & (BUFF_SOCKET|BUFF_FD)){
            close(bf->fd);
            Buff_UnsetFd(bf);
        }

        if((bf->type.state & ERROR) || (bf->type.state & (SUCCESS|END)) == 0){
            st->type.state |= ERROR;
            tsk->type.state |= ERROR;
            break;
        }
    }

    if((st->type.state & ERROR) == 0){
        st->type.state |= SUCCESS;
    }
    
    DebugStack_Pop();
    return st->type.state;
}

status TcpTask_ExpectRecv(Step *st, Task *tsk){
    struct pollfd *pfd = TcpTask_GetPollFd(tsk);
    pfd->events = POLLIN|POLLNVAL|POLLHUP|POLLERR;
    tsk->type.state |= TASK_UPDATE_CRIT;
    return tsk->type.state;
}

status TcpTask_ExpectSend(Step *st, Task *tsk){
    struct pollfd *pfd = TcpTask_GetPollFd(tsk);
    pfd->events = POLLOUT|POLLNVAL|POLLHUP|POLLERR;
    tsk->type.state |= TASK_UPDATE_CRIT;
    return tsk->type.state;
}
