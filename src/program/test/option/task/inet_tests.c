#include <external.h>
#include <caneka.h>
#include <test_module.h>

static status InetExample_finalize(Req *req){
    close(req->crit->pfd.fd);
    return SUCCESS;
}


static status InetExample_populate(MemCh *m, Req *req, void *arg, void *source){
    Debug_Push(m, req);
    /*

    struct pollfd *pfd = TcpTask_GetPollFd(tsk);
    Single *fdw = (Single *)Ifc(m, arg, TYPE_WRAPPED_I32);
    pfd->fd = fdw->val.i;

    HttpTask_InitResponse(tsk, NULL, source);
    Task_AddStep(tsk, WebServer_GatherPage, NULL, NULL, ZERO);
    HttpTask_AddRecieve(tsk, NULL, NULL);

    Buff *bf = Buff_Make(m, ZERO);
    Buff_Add(bf, S(m, "Hidy!"));

    Task_AddDataStep(tsk, InetExample_read, Sv(m, "Hidy!"), bf, NULL, ZERO);

    */
    Return(m, SUCCESS);
}

status Inet_Tests(MemCh *m){
    Debug_Push(m, NULL);

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
    
    Return(m, r);
}
