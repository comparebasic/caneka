#include <external.h>
#include <caneka.h>

static Task* tcpReqMake(MemCh *m, Task *tsk, Abstract *arg, Abstract *source){
    Task *child = Task_Make(NULL, NULL);
    HttpTask_InitResponse(child, arg, source);
    /* add app stuf here */
    HttpTask_AddRecieve(child, NULL, NULL);
    return child;
}

status ServeTcp_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    MemCh *m = MemCh_Make();
    status r = READY;
    Abstract *args[2];
    
    TcpCtx *ctx = TcpCtx_Make(m);
    ctx->port = 3000;
    ctx->func = TcpReqMake;

    Task *tsk ServeTcp_Make(ctx);
    Task_Tumble(tsk);

    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}
