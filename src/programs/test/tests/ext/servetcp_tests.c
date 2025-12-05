#include <external.h>
#include <caneka.h>
#include <test_module.h>

static status Example_log(Step *_st, Task *tsk){
    ProtoCtx *proto = (ProtoCtx *)as(tsk->data, TYPE_PROTO_CTX);
    HttpCtx *ctx = (HttpCtx *)as(proto->ctx, TYPE_HTTP_CTX);
    struct timespec now;
    Time_Now(&now);
    void *args[] = {
        Lookup_Get(HttpMethods, ctx->method),
        ctx->path,
        Time_ToStr(OutStream->m, &now),
        NULL,
    };

    return SUCCESS;
}

static status Example_populate(MemCh *m, Task *tsk, void *arg, void *source){
    struct pollfd *pfd = TcpTask_GetPollFd(tsk);
    Single *fdw = (Single *)as(arg, TYPE_WRAPPED_I32);
    pfd->fd = fdw->val.i;

    HttpTask_InitResponse(tsk, arg, source);
    /* add app stuf here */
    HttpTask_AddRecieve(tsk, NULL, NULL);
    return SUCCESS;
}

status ServeTcp_Tests(MemCh *m){
    DebugStack_Push(NULL, 0);
    status r = READY;
    
    TcpCtx *ctx = TcpCtx_Make(m);
    ctx->port = 3000;
    ctx->populate = Example_populate;
    ctx->finalize = Example_log;

    Task *tsk = ServeTcp_Make(ctx);
    Task_Tumble(tsk);

    DebugStack_Pop();
    return r;
}
