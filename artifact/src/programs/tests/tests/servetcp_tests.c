#include <external.h>
#include <caneka.h>

static status tcpReqPopulate(MemCh *m, Task *tsk, Abstract *arg, Abstract *source){
    struct pollfd *pfd = TcpTask_GetPollFd(tsk);
    Single *fdw = (Single *)as(arg, TYPE_WRAPPED_I32);
    pfd->fd = fdw->val.i;

    printf("init http request\n");
    fflush(stdout);
    HttpTask_InitResponse(tsk, arg, source);
    /* add app stuf here */
    HttpTask_AddRecieve(tsk, NULL, NULL);
    printf("Done init http request\n");
    fflush(stdout);
    return SUCCESS;
}

status ServeTcp_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    MemCh *m = MemCh_Make();
    status r = READY;
    Abstract *args[2];
    
    TcpCtx *ctx = TcpCtx_Make(m);
    ctx->port = 3000;
    ctx->func = tcpReqPopulate;

    Task *tsk = ServeTcp_Make(ctx);
    Task_Tumble(tsk);

    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}
