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
    struct pollfd *pfd = TcpTask_GetPollFd(tsk);
    ProtoCtx *proto = (ProtoCtx *)as(tsk->data, TYPE_PROTO_CTX);

    Buff *bf = (Buff *)as(st->data, TYPE_BUFF);
    Buff_SetFd(proto->out, pfd->fd);
    Buff_Pipe(proto->out, bf);
    if(bf->type.state & BUFF_FD){
        close(bf->fd);
        Buff_UnsetFd(bf);
    }

    if(bf->type.state & (SUCCESS|ERROR|END)){
        st->type.state |= SUCCESS;
    }
    
    DebugStack_Pop();
    return st->type.state;
}

status TcpTask_ExpectRecv(Step *st, Task *tsk){
    struct pollfd *pfd = TcpTask_GetPollFd(tsk);
    pfd->events = POLLIN;
    return TASK_UPDATE_CRIT;
}

status TcpTask_ExpectSend(Step *st, Task *tsk){
    struct pollfd *pfd = TcpTask_GetPollFd(tsk);
    pfd->events = POLLOUT;
    return TASK_UPDATE_CRIT;
}
