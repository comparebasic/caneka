#include <external.h>
#include <caneka.h>

status TcpTask_ReadToRbl(Step *st, Task *tsk){
    DebugStack_Push(st, st->type.of);
    struct pollfd *pfd = TcpTask_GetPollFd(tsk);
    ProtoCtx *proto = (ProtoCtx *)as(tsk->data, TYPE_PROTO_CTX);
    Roebling *rbl = (Roebling *)as(st->arg, TYPE_ROEBLING);

    Abstract *args[] = { NULL, NULL };
    Out("^c.^{STACK.name}^0\n", args);

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
        Out("^0.ReadToRbl -> ^c.&^0\n", args);
    }
    
    st->type.state |= (rbl->type.state & (SUCCESS|ERROR));
    if(st->type.state & SUCCESS && (tsk->type.state & DEBUG)){
        Abstract *args[] = {
            (Abstract *)tsk->data,
            NULL,
        };
        Out("^0.Parsed Tcp Initial Request -> ^c.&/@^0\n", args);
    }

    Out("^b.ReadToRbl^0\n", NULL);

    DebugStack_Pop();
    return st->type.state;
}

status TcpTask_WriteStep(Step *st, Task *tsk){
    DebugStack_Push(st, st->type.of);
    Abstract *args[3];
    struct pollfd *pfd = TcpTask_GetPollFd(tsk);
    ProtoCtx *proto = (ProtoCtx *)as(tsk->data, TYPE_PROTO_CTX);

    Buff *bf = (Buff *)as(st->data, TYPE_BUFF);
    Buff_SetFd(proto->out, pfd->fd);
    Buff_Pipe(proto->out, bf);

    if(bf->type.state & (SUCCESS|ERROR)){
        st->type.state |= SUCCESS;
    }

    args[0] = NULL;
    args[1] = (Abstract *)I64_Wrapped(tsk->m, bf->st.st_size);
    args[2] = NULL;
    Out("^c.^{STACK.name} sent:@^0\n", args);
    
    DebugStack_Pop();
    return st->type.state;
}

status TcpTask_ExpectRecv(Step *st, Task *tsk){
    struct pollfd *pfd = TcpTask_GetPollFd(tsk);
    pfd->events = POLL_IN;
    printf("SETTING IN!\n");
    return TASK_UPDATE_CRIT;
}

status TcpTask_ExpectSend(Step *st, Task *tsk){
    struct pollfd *pfd = TcpTask_GetPollFd(tsk);
    pfd->events = POLL_OUT;
    printf("Expect Send %d\n", pfd->fd);
    return TASK_UPDATE_CRIT;
}
