#include <external.h>
#include <caneka.h>

static status InetExample_finalize(Step *_st, Task *tsk){
    struct pollfd *pfd = TcpTask_GetPollFd(tsk);
    close(pfd->fd);
    return SUCCESS;
}

static status InetExample_send(Step *st, Task *tsk){
    Buff *content = as(st->data, TYPE_BUFF);
    StrVec *expected = as(st->arg, TYPE_STRVEC);
    MemCh *m = tsk->m;

    struct pollfd *pfd = TcpTask_GetPollFd(tsk);
    Buff *out = Buff_Make(m, BUFF_UNBUFFERED);
    Buff_SetSocket(out, pfd->fd);
    Buff_Pipe(out, content);
    if(content->unsent.total == 0){
        st->type.state |= SUCCESS;
    }
    return st->type.state;
}

static status InetExample_read(Step *st, Task *tsk){
    Buff *in = as(st->data, TYPE_BUFF);
    StrVec *expected = as(st->arg, TYPE_STRVEC);
    Buff_ReadAmount(in, 1024);
    MemCh *m = tsk->m;

    if(Equals(in->v, expected)){
        Task_AddDataStep(tsk,
            InetExample_send, S(m, "Ho!"), Buff_Make(m, BUFF_UNBUFFERED), NULL, ZERO);
        TcpTask_ExpectSend(NULL, tsk);
        st->type.state |= SUCCESS;
    }

    return st->type.state;
}

static status InetExample_populate(MemCh *m, Task *tsk, void *arg, void *source){
    DebugStack_Push(tsk, tsk->type.of);
    struct pollfd *pfd = TcpTask_GetPollFd(tsk);
    Single *fdw = (Single *)as(arg, TYPE_WRAPPED_I32);
    pfd->fd = fdw->val.i;

    HttpTask_InitResponse(tsk, NULL, source);
    Task_AddStep(tsk, WebServer_GatherPage, NULL, NULL, ZERO);
    HttpTask_AddRecieve(tsk, NULL, NULL);

    Buff *bf = Buff_Make(m, ZERO);
    Buff_Add(bf, S(m, "Hidy!"));

    Task_AddDataStep(tsk, InetExample_read, Sv(m, "Hidy!"), bf, NULL, ZERO);

    DebugStack_Pop();
    return SUCCESS;
}

status Inet_Tests(MemCh *m){
    DebugStack_Push(NULL, 0);
    status r = READY;
    void *args[5];

    Str *s = S(m, "10.1.255.7");
    quad ip4 = 0;
    byte *ib = (byte *)&ip4;
    ib[0] = 10;
    ib[1] = 1;
    ib[2] = 255;
    ib[3] = 7;

    Str *returned = Ip4_ToStr(m, ip4);
    args[0] = s;
    args[1] = returned;
    args[2] = NULL;
    r |= Test(Equals(args[0], args[1]),
        "From Str to quad and back matches, expected $, have $", args);

    quad returnedIp4 = Str_ToIp4(m, s);
    args[1] = Str_Ref(m, (byte *)&returnedIp4, sizeof(quad), sizeof(quad), STRING_BINARY);
    args[0] = Str_Ref(m, (byte *)&ip4, sizeof(quad), sizeof(quad), STRING_BINARY);
    args[2] = NULL;
    r |= Test(returnedIp4 == ip4,
        "From quad to Str matches, expected &, have &", args);

    returnedIp4 = Quad_ToIp4(10, 1, 255, 7);
    args[1] = Str_Ref(m, (byte *)&returnedIp4, sizeof(quad), sizeof(quad), STRING_BINARY);
    args[0] = Str_Ref(m, (byte *)&ip4, sizeof(quad), sizeof(quad), STRING_BINARY);
    args[2] = NULL;
    r |= Test(returnedIp4 == ip4,
        "From four sepeate bytes to quad matches, expected &, have &", args);


    /*
    util ip6[2] = {0, 0};
    TcpCtx *ctx = TcpCtx_Make(m);
    Task *srv = ServeTcp_Make(NULL);
    ctx->port = 4000;
    ctx->populate = InetExample_populate;
    ctx->finalize = InetExample_finalize;
    srv->source = (Abstract *)ctx;
    Task_Tumble(srv);
    */

    /* add spawn tasks to spawn processes to hit the network server */
    
    DebugStack_Pop();
    return r;
}
